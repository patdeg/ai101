#!/usr/bin/env python3
"""
Example 18 (NPU Variant): Real-Time Pose Estimation on Orange Pi 5 Plus NPU

WHAT THIS DEMONSTRATES:
    - Running AI pose estimation on a dedicated NPU (Neural Processing Unit)
    - Using the CIX NOE_Engine to run inference on specialized hardware
    - Manual preprocessing: letterbox resize, color conversion, normalization
    - Manual output parsing: extracting bounding boxes and keypoints from raw tensors
    - Non-Maximum Suppression (NMS) to filter overlapping detections
    - Drawing skeleton overlays on camera frames in real-time

WHAT YOU'LL LEARN:
    - What an NPU is and how it differs from a CPU and GPU
    - Why NPU models use .cix format instead of .pt (PyTorch) format
    - The full inference pipeline that libraries like ultralytics hide from you:
        1. Preprocess the image (resize, pad, normalize, reorder channels)
        2. Run the model (NPU inference)
        3. Parse the raw output tensor (extract boxes, scores, keypoints)
        4. Apply Non-Maximum Suppression (remove duplicate detections)
        5. Scale coordinates back to the original image size
    - How letterbox resizing preserves aspect ratio with padding
    - What CHW vs HWC means (channel ordering for image data)

PREREQUISITES:
    - Orange Pi 5 Plus with CIX NPU
    - CIX AI Model Hub installed at /opt/ai/ai_model_hub_25_Q3/
    - A webcam connected to the Orange Pi
    - Python packages: opencv-python, numpy
    - The NOE_Engine library (comes with the CIX SDK)

EXPECTED OUTPUT:
    - A live video window showing your webcam feed
    - Colored dots on each detected body joint (keypoint)
    - Lines connecting the joints to form a skeleton
    - FPS counter in the top-left corner
    - Press 'q' to quit the program

Hardware: Orange Pi 5 Plus (https://www.amazon.com/dp/B0G3P8VFHK)
Run with: python3 18_pose_estimation_npu.py
"""

# ==============================================================================
# Step 1: Import libraries
# ==============================================================================

# OpenCV - for camera capture, image manipulation, drawing, and display windows.
# This is the same library used in the GPU version.
import cv2

# NumPy - for working with arrays of numbers. Images are stored as NumPy arrays,
# and the NPU model's input and output are also NumPy arrays.
import numpy as np

# For measuring frames per second (FPS) to see how fast the NPU runs.
import time

# sys lets us modify the Python module search path so we can find the NOE_Engine
# library that is installed with the CIX AI Model Hub.
import sys

# ==============================================================================
# Step 2: Load the NPU model
# ==============================================================================

# The CIX AI Model Hub is a collection of pre-compiled AI models for the NPU.
# It's installed at this path on the Orange Pi. We need to add it to Python's
# search path so we can import the NOE_Engine library from it.
CIX_MODEL_HUB = "/opt/ai/ai_model_hub_25_Q3"
sys.path.insert(0, CIX_MODEL_HUB)

# NOE_Engine is the CIX library for running AI models on the NPU chip.
# EngineInfer is the class that loads a model and runs inference.
# Think of it like the YOLO() class from ultralytics, but for NPU hardware.
from utils.NOE_Engine import EngineInfer

# Path to the pre-compiled YOLOv8s-pose model for the NPU.
#
# Key difference from the GPU version:
#   GPU version uses: yolov8s-pose.pt  (PyTorch format, runs on CPU/GPU)
#   NPU version uses: yolov8s-pose.cix (CIX format, runs on NPU chip)
#
# A .cix file is a model that has been "compiled" specifically for the NPU chip.
# Just like how a .exe is compiled for a specific CPU, a .cix is compiled for
# the CIX NPU. This compilation step converts the model's math operations into
# instructions that the NPU hardware understands natively.
#
# Why not just use .pt files on the NPU?
#   .pt files contain PyTorch operations designed for CPU/GPU.
#   The NPU has completely different hardware, so it needs its own format.
#   The compilation process optimizes the model for the NPU's architecture.
MODEL_PATH = (
    f"{CIX_MODEL_HUB}/models/ComputeVision/Pose_Estimation/"
    f"onnx_yolov8s_pose/yolov8s-pose.cix"
)

# What is an NPU?
#   CPU = Central Processing Unit:  Good at everything, not specialized
#   GPU = Graphics Processing Unit:  Great at parallel math (thousands of cores)
#   NPU = Neural Processing Unit:   Purpose-built for AI inference only
#
# The NPU on the Orange Pi 5 Plus is a dedicated chip that can ONLY run AI
# models, but it does so very efficiently - using less power than a GPU while
# still being much faster than a CPU for AI tasks.

print("Loading YOLOv8s-pose model onto NPU...")
model = EngineInfer(MODEL_PATH)
print("Model loaded successfully!\n")

# The model expects input images of exactly 640x640 pixels.
# All YOLO models use this fixed size. We'll need to resize camera frames
# to match this before running inference.
MODEL_SIZE = 640

# ==============================================================================
# Step 3: Define the preprocessing function
# ==============================================================================

# In the GPU version, ultralytics handles all preprocessing automatically.
# With the NPU, we must do it ourselves. This is what happens "under the hood"
# in every AI vision system.
#
# The preprocessing pipeline has 4 steps:
#   1. Letterbox resize: Scale the image to 640x640 while keeping aspect ratio
#   2. Color conversion: Convert BGR (OpenCV format) to RGB (model format)
#   3. Normalize: Scale pixel values from 0-255 to 0.0-1.0
#   4. Transpose: Rearrange from HWC to CHW format (explained below)

def preprocess_frame(frame):
    """
    Prepare a camera frame for the NPU model.

    Args:
        frame: A BGR image from OpenCV, shape (height, width, 3)

    Returns:
        input_data: Preprocessed array ready for the NPU, shape (1, 3, 640, 640)
        ratio: How much we scaled the image (needed to convert coordinates back)
        pad_w: Horizontal padding added (needed to convert coordinates back)
        pad_h: Vertical padding added (needed to convert coordinates back)
    """

    # Get the original image dimensions
    orig_h, orig_w = frame.shape[:2]

    # --- Step 3a: Letterbox resize ---
    #
    # The model needs exactly 640x640 input, but camera frames might be
    # 1920x1080 or 640x480 (not square). We can't just stretch the image
    # to 640x640 because that would distort people's proportions.
    #
    # Instead, we use "letterbox" resizing:
    #   1. Scale the image so the longest side fits in 640 pixels
    #   2. Add gray padding to fill the remaining space
    #
    # This is like how movies have black bars on top/bottom when shown on
    # a different aspect ratio screen - the content isn't distorted.

    # Calculate the scale factor to fit the image within 640x640
    ratio = min(MODEL_SIZE / orig_h, MODEL_SIZE / orig_w)

    # Calculate the new image size (before padding)
    new_w = int(round(orig_w * ratio))
    new_h = int(round(orig_h * ratio))

    # Calculate padding needed on each side to reach 640x640
    pad_w = (MODEL_SIZE - new_w) / 2  # Padding on left and right
    pad_h = (MODEL_SIZE - new_h) / 2  # Padding on top and bottom

    # Resize the image to the new dimensions
    if (orig_w, orig_h) != (new_w, new_h):
        frame = cv2.resize(frame, (new_w, new_h), interpolation=cv2.INTER_LINEAR)

    # Add gray padding around the resized image to make it exactly 640x640
    # Gray (114, 114, 114) is used because it's close to the average pixel value
    # in the training dataset, which helps the model perform better.
    top = int(round(pad_h - 0.1))
    bottom = int(round(pad_h + 0.1))
    left = int(round(pad_w - 0.1))
    right = int(round(pad_w + 0.1))
    frame = cv2.copyMakeBorder(
        frame, top, bottom, left, right,
        cv2.BORDER_CONSTANT, value=(114, 114, 114)
    )

    # --- Step 3b: BGR to RGB + Step 3c: Normalize + Step 3d: Transpose ---
    #
    # Now we need to convert the image format for the model:
    #
    # Transpose: HWC → CHW
    #   Camera gives us: (Height, Width, Channels) = (640, 640, 3)
    #   Model expects:   (Channels, Height, Width) = (3, 640, 640)
    #
    #   Why? The model was designed to process all red pixels together, then
    #   all green, then all blue. This "channels first" format (CHW) is more
    #   efficient for neural networks. It's like sorting a deck of cards by
    #   suit rather than keeping them in dealt order.
    #
    # BGR to RGB: The [::-1] reverses the channel order.
    #   OpenCV gives us channels as [Blue, Green, Red]
    #   The model expects channels as [Red, Green, Blue]
    #
    # Normalize: Divide by 255 to convert pixel values from 0-255 integers
    #   to 0.0-1.0 floats. Neural networks work better with small numbers.

    # np.einsum("HWC->CHW", frame) rearranges dimensions: (H,W,C) → (C,H,W)
    # [::-1] reverses channels: BGR → RGB
    # / 255.0 normalizes pixel values to [0, 1]
    img = np.ascontiguousarray(
        np.einsum("HWC->CHW", frame)[::-1], dtype=np.float32
    ) / 255.0

    # Add batch dimension: (3, 640, 640) → (1, 3, 640, 640)
    # The "1" means we're processing 1 image at a time.
    # Models are designed to process batches of images, but we just send one.
    input_data = img[None]

    return input_data, ratio, pad_w, pad_h


# ==============================================================================
# Step 4: Define Non-Maximum Suppression (NMS)
# ==============================================================================

# The model outputs MANY detections for each person - sometimes hundreds of
# overlapping bounding boxes for the same person. NMS filters these down to
# keep only the best detection for each person.
#
# How NMS works (step by step):
#   1. Sort all detections by confidence score (highest first)
#   2. Take the highest-scoring detection - this is definitely a "keeper"
#   3. Compare it against all remaining detections
#   4. Remove any detection that overlaps too much with our keeper
#      (they're probably detecting the same person)
#   5. Repeat from step 2 with the remaining detections
#
# "Overlaps too much" is measured by IoU (Intersection over Union):
#   IoU = (area where boxes overlap) / (total area of both boxes combined)
#   IoU > 0.6 means the boxes overlap more than 60% → probably same person

def nms(detections, iou_threshold):
    """
    Non-Maximum Suppression: remove duplicate detections of the same person.

    Args:
        detections: Array of detections, each row is [x1, y1, x2, y2, score, ...]
        iou_threshold: Maximum allowed overlap (0.6 = 60%)

    Returns:
        List of the best detections with duplicates removed.
    """
    # Extract coordinates and scores from all detections
    x1 = detections[:, 0]  # Left edge of each box
    y1 = detections[:, 1]  # Top edge of each box
    x2 = detections[:, 2]  # Right edge of each box
    y2 = detections[:, 3]  # Bottom edge of each box
    scores = detections[:, 4]  # Confidence score of each detection

    # Calculate the area of each bounding box
    areas = (x2 - x1 + 1) * (y2 - y1 + 1)

    # Sort detection indices by score (highest confidence first)
    order = scores.argsort()[::-1]

    keep = []  # List of detection indices we want to keep

    while order.size > 0:
        # Take the detection with the highest remaining score
        i = order[0]
        keep.append(i)

        # Calculate how much each remaining box overlaps with our keeper
        # np.maximum/minimum clips coordinates to find the intersection rectangle
        xx1 = np.maximum(x1[i], x1[order[1:]])
        yy1 = np.maximum(y1[i], y1[order[1:]])
        xx2 = np.minimum(x2[i], x2[order[1:]])
        yy2 = np.minimum(y2[i], y2[order[1:]])

        # Width and height of the intersection (0 if no overlap)
        w = np.maximum(0.0, xx2 - xx1 + 1)
        h = np.maximum(0.0, yy2 - yy1 + 1)

        # Intersection over Union (IoU)
        intersection = w * h
        iou = intersection / (areas[i] + areas[order[1:]] - intersection)

        # Keep only detections with low overlap (different people)
        remaining = np.where(iou <= iou_threshold)[0]
        order = order[remaining + 1]  # +1 because we skipped index 0

    return [detections[i] for i in keep]


# ==============================================================================
# Step 5: Define output parsing function
# ==============================================================================

# The NPU model outputs a raw tensor (big array of numbers) that we need to
# decode. This is another thing that ultralytics handles automatically in the
# GPU version, but we do manually here.
#
# The raw output shape is (1, 56, 8400):
#   - 1 = batch size (we sent 1 image)
#   - 56 = values per detection: 4 (bbox) + 1 (confidence) + 51 (17 keypoints × 3)
#   - 8400 = number of possible detection locations in the image
#       The model checks 8400 different positions/sizes for people
#
# Each detection's 56 values break down as:
#   [0:4]  = bounding box: center_x, center_y, width, height
#   [4]    = confidence score (0.0 to 1.0)
#   [5:56] = 17 keypoints, each with (x, y, confidence) = 51 values

# Confidence threshold: only keep detections above this score
CONF_THRESHOLD = 0.5

# NMS threshold: remove boxes that overlap more than this
NMS_THRESHOLD = 0.6

def parse_npu_output(raw_output, ratio, pad_w, pad_h, orig_shape):
    """
    Parse raw NPU output into usable bounding boxes and keypoints.

    Args:
        raw_output: Raw tensor from the NPU, shape (1, 56, 8400)
        ratio: Scale factor from preprocessing (to convert coordinates back)
        pad_w: Horizontal padding from preprocessing
        pad_h: Vertical padding from preprocessing
        orig_shape: (height, width) of the original camera frame

    Returns:
        List of (bbox, keypoints) tuples for each detected person.
        bbox = (x1, y1, x2, y2) in original image coordinates.
        keypoints = array of shape (17, 3) with (x, y, confidence) per joint.
    """

    # Reshape from (1, 56, 8400) to (8400, 56)
    # This gives us 8400 rows, each containing one possible detection
    pred = np.reshape(raw_output, (1, 56, 8400))[0]
    pred = np.transpose(pred, (1, 0))  # (56, 8400) → (8400, 56)

    # Filter by confidence: only keep detections the model is sure about
    # This typically reduces 8400 candidates down to just a few
    pred = pred[pred[:, 4] > CONF_THRESHOLD]

    persons = []

    if len(pred) == 0:
        return persons  # No people detected in this frame

    # --- Convert bounding box format ---
    # Model outputs: center_x, center_y, width, height (xywh)
    # We need:       left, top, right, bottom (xyxy)
    # This conversion is needed because NMS and drawing use corner coordinates.
    boxes = pred.copy()
    boxes[:, 0] = pred[:, 0] - pred[:, 2] / 2  # x1 = center_x - width/2
    boxes[:, 1] = pred[:, 1] - pred[:, 3] / 2  # y1 = center_y - height/2
    boxes[:, 2] = pred[:, 0] + pred[:, 2] / 2  # x2 = center_x + width/2
    boxes[:, 3] = pred[:, 1] + pred[:, 3] / 2  # y2 = center_y + height/2

    # --- Apply Non-Maximum Suppression ---
    # This removes duplicate detections of the same person
    boxes = nms(boxes, NMS_THRESHOLD)
    boxes = np.array(boxes)

    # --- Scale coordinates back to original image size ---
    # During preprocessing, we resized and padded the image.
    # Now we need to reverse that to get coordinates in the original frame.
    # 1. Subtract the padding offset
    # 2. Divide by the scale ratio

    # Convert xyxy back to xywh for scaling (the scale function expects xywh)
    boxes[:, 2] = boxes[:, 2] - boxes[:, 0]  # width = x2 - x1
    boxes[:, 3] = boxes[:, 3] - boxes[:, 1]  # height = y2 - y1

    # Scale bounding box coordinates
    boxes[:, 0] -= pad_w       # Remove horizontal padding
    boxes[:, 1] -= pad_h       # Remove vertical padding
    boxes[:, :4] /= ratio      # Undo the resize scaling

    # Scale keypoint coordinates (indices 5 onwards)
    # Each keypoint has 3 values: x, y, confidence
    # We only need to scale x and y, not confidence
    for kpt_idx in range(17):
        x_col = 5 + kpt_idx * 3       # Column index for this keypoint's x
        y_col = 5 + kpt_idx * 3 + 1   # Column index for this keypoint's y
        boxes[:, x_col] = (boxes[:, x_col] - pad_w) / ratio
        boxes[:, y_col] = (boxes[:, y_col] - pad_h) / ratio

    # Clip bounding boxes to image boundaries (prevent drawing outside frame)
    boxes[:, 0] = boxes[:, 0].clip(0, orig_shape[1])  # x1
    boxes[:, 1] = boxes[:, 1].clip(0, orig_shape[0])  # y1

    # --- Extract person data ---
    for box in boxes:
        # Bounding box: x, y, width, height
        x, y, w, h = box[0:4]
        bbox = (float(x), float(y), float(x + w), float(y + h))

        # Keypoints: reshape the 51 values into (17, 3) array
        # Each row is [x_position, y_position, confidence]
        keypoints = box[5:].reshape(17, 3)

        persons.append((bbox, keypoints))

    return persons


# ==============================================================================
# Step 6: Open the webcam
# ==============================================================================

# cv2.VideoCapture(0) opens your default webcam (camera index 0).
# On the Orange Pi, you might need to use a different index depending on
# how many cameras are connected. USB cameras are typically /dev/video0.

cap = cv2.VideoCapture(0)

# Check if the camera opened successfully
if not cap.isOpened():
    print("Error: Could not open webcam")
    print("Make sure a camera is connected to the Orange Pi")
    print("Try: ls /dev/video* to see available cameras")
    exit(1)

# Read the camera's resolution
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
print(f"Camera opened: {frame_width}x{frame_height} pixels")
print("Press 'q' to quit\n")

# ==============================================================================
# Step 7: Define skeleton connections (which joints connect to which)
# ==============================================================================

# The COCO pose model detects 17 keypoints on the human body.
# Each keypoint has an index number (0-16), an x/y position, and a confidence
# score (0.0 to 1.0) indicating how sure the AI is about that detection.
#
# Here are all 17 keypoints and their index numbers:
#
#   Index   Body Part           Index   Body Part
#   -----   ---------           -----   ---------
#     0     Nose                  9     Left wrist
#     1     Left eye             10     Right wrist
#     2     Right eye            11     Left hip
#     3     Left ear             12     Right hip
#     4     Right ear            13     Left knee
#     5     Left shoulder        14     Right knee
#     6     Right shoulder       15     Left ankle
#     7     Left elbow           16     Right ankle
#     8     Right elbow
#
# Note: "Left" and "Right" are from the PERSON's perspective, not yours.
# When looking at the camera, their left appears on your right side.

# Define which keypoints should be connected by lines to form the skeleton.
# Each tuple is (start_keypoint_index, end_keypoint_index).
# For example, (5, 7) connects left shoulder to left elbow.

SKELETON_CONNECTIONS = [
    # Face connections
    (0, 1),   # Nose -> Left eye
    (0, 2),   # Nose -> Right eye
    (1, 3),   # Left eye -> Left ear
    (2, 4),   # Right eye -> Right ear

    # Upper body - left side
    (5, 7),   # Left shoulder -> Left elbow
    (7, 9),   # Left elbow -> Left wrist

    # Upper body - right side
    (6, 8),   # Right shoulder -> Right elbow
    (8, 10),  # Right elbow -> Right wrist

    # Torso (the body's core)
    (5, 6),   # Left shoulder -> Right shoulder
    (5, 11),  # Left shoulder -> Left hip
    (6, 12),  # Right shoulder -> Right hip
    (11, 12), # Left hip -> Right hip

    # Lower body - left side
    (11, 13), # Left hip -> Left knee
    (13, 15), # Left knee -> Left ankle

    # Lower body - right side
    (12, 14), # Right hip -> Right knee
    (14, 16), # Right knee -> Right ankle
]

# Minimum confidence to draw a keypoint or connection.
# 0.5 means the AI must be at least 50% confident the keypoint is correct.
KEYPOINT_CONF_THRESHOLD = 0.5

# Colors for drawing - OpenCV uses BGR format, NOT RGB!
# BGR = Blue, Green, Red (reversed from the usual RGB order).
KEYPOINT_COLOR = (0, 255, 0)    # Green dots for joints
SKELETON_COLOR = (255, 255, 0)  # Cyan lines for bones (Blue=255 + Green=255)
TEXT_COLOR = (0, 255, 255)      # Yellow text for FPS counter

# ==============================================================================
# Step 8: Main loop - read frames, preprocess, NPU inference, draw, display
# ==============================================================================

# This is the same structure as the GPU version, but with explicit steps:
#   GPU version:  frame → model(frame) → draw results
#   NPU version:  frame → preprocess → model.forward → parse output → draw results
#
# The NPU version shows you everything that happens inside model(frame)!

# Track time between frames to calculate FPS
prev_time = time.time()

# This loop runs continuously until the user presses 'q'.
while True:

    # --- Read one frame from the camera ---
    ret, frame = cap.read()

    if not ret:
        print("Error: Could not read frame from camera")
        break

    # --- Preprocess the frame for the NPU ---
    # (In the GPU version, ultralytics does this automatically)
    input_data, ratio, pad_w, pad_h = preprocess_frame(frame)

    # --- Run inference on the NPU chip ---
    # model.forward() sends the preprocessed image to the NPU hardware,
    # which runs the neural network and returns raw output tensors.
    # This is the actual AI computation happening on the dedicated NPU chip.
    raw_output = model.forward([input_data])[0]

    # --- Parse the raw output into usable detections ---
    # (In the GPU version, ultralytics does this automatically too)
    persons = parse_npu_output(
        raw_output, ratio, pad_w, pad_h, frame.shape[:2]
    )

    # --- Draw skeletons for each detected person ---
    for bbox, keypoints in persons:

        # Draw skeleton lines first (so dots appear on top of lines)
        for start_idx, end_idx in SKELETON_CONNECTIONS:
            # Only draw the line if BOTH endpoints have high enough confidence
            if (keypoints[start_idx][2] > KEYPOINT_CONF_THRESHOLD and
                    keypoints[end_idx][2] > KEYPOINT_CONF_THRESHOLD):
                # Get the x,y positions and convert to integers (pixels)
                start_point = (int(keypoints[start_idx][0]),
                               int(keypoints[start_idx][1]))
                end_point = (int(keypoints[end_idx][0]),
                             int(keypoints[end_idx][1]))
                # Draw a line between the two keypoints
                cv2.line(frame, start_point, end_point, SKELETON_COLOR, 2)

        # Draw keypoint dots on top of the skeleton lines
        for kp_idx in range(17):
            if keypoints[kp_idx][2] > KEYPOINT_CONF_THRESHOLD:
                x = int(keypoints[kp_idx][0])
                y = int(keypoints[kp_idx][1])
                # Draw a filled circle at the keypoint position
                cv2.circle(frame, (x, y), 5, KEYPOINT_COLOR, -1)

    # --- Calculate and display FPS ---
    current_time = time.time()
    fps = 1.0 / max(current_time - prev_time, 0.001)
    prev_time = current_time

    # Draw FPS text and "NPU" label so you know which hardware is running
    cv2.putText(frame, f"FPS: {fps:.1f} (NPU)", (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX, 1.0, TEXT_COLOR, 2)

    # --- Show the frame in a window ---
    cv2.imshow("Pose Estimation (NPU)", frame)

    # --- Check if user pressed 'q' to quit ---
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# ==============================================================================
# Step 9: Cleanup - release resources
# ==============================================================================

# Release the camera so other programs can use it
cap.release()

# Close all OpenCV display windows
cv2.destroyAllWindows()

# Release the NPU model resources.
# This is important on the NPU because the hardware has limited memory.
# On GPU, Python's garbage collector usually handles this automatically,
# but NPU resources must be explicitly freed.
model.clean()

print("\nPose estimation stopped. NPU resources released. Goodbye!")

# ==============================================================================
# Reference: Key Concepts Explained
# ==============================================================================

# NPU vs GPU vs CPU for AI:
#   CPU (Central Processing Unit):
#     - General purpose, handles everything: OS, apps, networking
#     - Can run AI models, but slowly (processes operations one by one)
#     - Every computer has one
#
#   GPU (Graphics Processing Unit):
#     - Originally for rendering graphics/games
#     - Thousands of small cores that work in parallel
#     - Excellent for AI because neural networks are mostly parallel math
#     - Uses more power (50-350 watts for dedicated GPUs)
#     - Examples: NVIDIA RTX 3090, RTX 4090
#
#   NPU (Neural Processing Unit):
#     - Purpose-built ONLY for running neural networks
#     - Extremely power-efficient (a few watts)
#     - Cannot do general computing, only AI inference
#     - Built into the SoC (System on Chip) alongside the CPU
#     - Examples: CIX NPU (Orange Pi), Apple Neural Engine, Google TPU
#
# Why use an NPU?
#   - Power efficiency: runs AI at a fraction of GPU power consumption
#   - Cost: Orange Pi 5 Plus (~$150) vs GPU workstation (~$2000+)
#   - Size: fits in a small board, great for embedded/edge applications
#   - Always-on AI: low power means it can run AI 24/7
#
# Model Format Comparison:
#   .pt  (PyTorch):     Generic, runs on CPU/GPU via PyTorch framework
#   .onnx (ONNX):       Intermediate format, portable between frameworks
#   .cix (CIX):         Compiled specifically for CIX NPU hardware
#   .engine (TensorRT): Compiled specifically for NVIDIA GPU hardware
#
#   The compilation process (.pt → .onnx → .cix) optimizes the model's
#   operations for the target hardware, making inference much faster.
#
# What We Do Manually (that ultralytics handles automatically):
#   1. Preprocessing:
#      - Letterbox resize (preserve aspect ratio with padding)
#      - BGR → RGB color conversion
#      - Normalize pixels from 0-255 to 0.0-1.0
#      - Transpose from HWC to CHW format
#      - Add batch dimension
#
#   2. Postprocessing:
#      - Reshape raw output tensor
#      - Filter by confidence threshold
#      - Convert bbox format (center → corners)
#      - Apply Non-Maximum Suppression
#      - Scale coordinates back to original image
#
#   Understanding these steps is valuable because:
#   - You learn what AI inference actually involves
#   - You can debug issues when things go wrong
#   - You can optimize performance by tweaking each step
#   - You can port models to any hardware, not just ones with libraries
#
# Letterbox Resizing Explained:
#   Camera frame (1920x1080):     After letterbox to 640x640:
#   +-------------------+         +-------------------+
#   |                   |         |   gray padding    |
#   |    actual image   |         +-------------------+
#   |                   |         |   resized image   |
#   +-------------------+         +-------------------+
#                                 |   gray padding    |
#                                 +-------------------+
#
#   The image is scaled to fit inside 640x640 without distortion.
#   Gray bars fill the remaining space. This preserves the correct
#   proportions of people's bodies for accurate pose detection.
#
# HWC vs CHW:
#   HWC = Height, Width, Channels: how OpenCV stores images
#     Shape: (480, 640, 3) → 480 rows, 640 columns, 3 colors (BGR)
#     Layout: pixel by pixel, each with all 3 color values
#
#   CHW = Channels, Height, Width: how neural networks expect images
#     Shape: (3, 480, 640) → 3 color planes, each 480x640
#     Layout: all red values, then all green values, then all blue values
#
#   Neural networks prefer CHW because they process one color channel
#   at a time through convolutional filters. It's more memory-efficient
#   for the way neural network math works.
#
# Raw Output Tensor (1, 56, 8400):
#   The model checks 8400 positions in the image for people.
#   8400 = 80×80 + 40×40 + 20×20 (three detection scales)
#     80×80 = 6400 positions for detecting small people
#     40×40 = 1600 positions for medium-sized people
#     20×20 =  400 positions for large/close people
#
#   For each position, it outputs 56 values:
#     [0:4]   = bounding box (center_x, center_y, width, height)
#     [4]     = person confidence score
#     [5:8]   = keypoint 0 (nose): x, y, confidence
#     [8:11]  = keypoint 1 (left eye): x, y, confidence
#     ...     = (continues for all 17 keypoints)
#     [53:56] = keypoint 16 (right ankle): x, y, confidence
#
# Try these experiments:
#   1. Change CONF_THRESHOLD to 0.3 - notice more (noisier) detections
#   2. Change NMS_THRESHOLD to 0.3 - stricter duplicate removal
#   3. Print fps each frame to compare NPU speed vs the GPU version
#   4. Add cv2.flip(frame, 1) before preprocess_frame() to mirror the image
#   5. Time just the model.forward() call to see pure NPU inference speed:
#        t0 = time.time()
#        raw_output = model.forward([input_data])[0]
#        print(f"NPU inference: {(time.time()-t0)*1000:.1f} ms")
