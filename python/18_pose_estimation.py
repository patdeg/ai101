#!/usr/bin/env python3
"""
Example 18: Real-Time Pose Estimation with YOLOv8 and Webcam

WHAT THIS DEMONSTRATES:
    - How AI can detect human body poses in real-time video
    - Using a pre-trained pose estimation model (YOLOv8s-pose)
    - Drawing skeleton overlays on camera frames
    - Processing video frame-by-frame in a loop
    - Measuring performance with an FPS counter

WHAT YOU'LL LEARN:
    - What pose estimation is and how it works
    - The COCO 17-keypoint body model (nose, eyes, shoulders, etc.)
    - How to load and run a YOLO model for inference
    - OpenCV basics: camera capture, drawing, display windows
    - The difference between RGB and BGR color formats
    - Real-time video processing loop pattern

PREREQUISITES:
    - Python 3.8 or higher
    - A webcam connected to your computer
    - Install dependencies:
        pip install ultralytics opencv-python numpy

EXPECTED OUTPUT:
    - A live video window showing your webcam feed
    - Colored dots on each detected body joint (keypoint)
    - Lines connecting the joints to form a skeleton
    - FPS counter in the top-left corner
    - Press 'q' to quit the program

Run with: python3 18_pose_estimation.py
"""

import cv2      # OpenCV - for camera, drawing, and display
import numpy as np  # NumPy - for array math (keypoints are arrays)
import time     # For measuring FPS (frames per second)

# We import YOLO from ultralytics - the library that provides the AI model
from ultralytics import YOLO

# ==============================================================================
# Step 1: Load the AI pose estimation model
# ==============================================================================

# YOLO stands for "You Only Look Once" - it's a family of AI models that can
# detect objects (and body poses) in images very quickly. The "s" in "yolov8s"
# means "small" - it's fast but less accurate than larger versions.
#
# A .pt file is a PyTorch model file. Think of it like a brain that has already
# been trained to recognize human body poses. The first time you run this, it
# will download the model file (~12 MB). After that, it loads from disk.
#
# Model sizes: yolov8n-pose (nano/fastest) < yolov8s-pose < yolov8m-pose
#              < yolov8l-pose < yolov8x-pose (extra-large/most accurate)

print("Loading YOLOv8s-pose model...")
print("(First run will download ~12 MB model file)\n")

# Load the pre-trained pose estimation model
model = YOLO("yolov8s-pose.pt")

print("Model loaded successfully!\n")

# ==============================================================================
# Step 2: Open the webcam
# ==============================================================================

# cv2.VideoCapture(0) opens your default webcam (camera index 0).
# If you have multiple cameras, try 1, 2, etc. for other cameras.
#
# A video is just a series of still images called "frames" shown rapidly
# (typically 30 frames per second). We read one frame at a time in a loop.

cap = cv2.VideoCapture(0)

# Check if the camera opened successfully
if not cap.isOpened():
    print("Error: Could not open webcam")
    print("Make sure a camera is connected and not in use by another app")
    exit(1)

# Read the camera's resolution so we know how big each frame is
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
print(f"Camera opened: {frame_width}x{frame_height} pixels")
print("Press 'q' to quit\n")

# ==============================================================================
# Step 3: Define the skeleton connections (which joints connect to which)
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
    (0, 1),   # Nose → Left eye
    (0, 2),   # Nose → Right eye
    (1, 3),   # Left eye → Left ear
    (2, 4),   # Right eye → Right ear

    # Upper body - left side
    (5, 7),   # Left shoulder → Left elbow
    (7, 9),   # Left elbow → Left wrist

    # Upper body - right side
    (6, 8),   # Right shoulder → Right elbow
    (8, 10),  # Right elbow → Right wrist

    # Torso (the body's core)
    (5, 6),   # Left shoulder → Right shoulder
    (5, 11),  # Left shoulder → Left hip
    (6, 12),  # Right shoulder → Right hip
    (11, 12), # Left hip → Right hip

    # Lower body - left side
    (11, 13), # Left hip → Left knee
    (13, 15), # Left knee → Left ankle

    # Lower body - right side
    (12, 14), # Right hip → Right knee
    (14, 16), # Right knee → Right ankle
]

# Minimum confidence to draw a keypoint or connection.
# 0.5 means the AI must be at least 50% confident the keypoint is correct.
# Lower values show more points but with more errors (false positives).
# Higher values show fewer points but more reliable ones.
CONFIDENCE_THRESHOLD = 0.5

# Colors for drawing - OpenCV uses BGR format, NOT RGB!
# BGR = Blue, Green, Red (reversed from the usual RGB order).
# This is a historical quirk of OpenCV from the 1990s.
# So (0, 255, 0) is pure GREEN in BGR, not red.
KEYPOINT_COLOR = (0, 255, 0)    # Green dots for joints
SKELETON_COLOR = (255, 255, 0)  # Cyan lines for bones (Blue=255 + Green=255)
TEXT_COLOR = (0, 255, 255)      # Yellow text for FPS counter

# ==============================================================================
# Step 4: Main loop - read frames, detect poses, draw skeleton, display
# ==============================================================================

# Track time between frames to calculate FPS (frames per second).
# FPS tells you how smoothly the video runs. 30 FPS = smooth, 10 FPS = choppy.
prev_time = time.time()

# This loop runs continuously until the user presses 'q'.
# Each iteration: capture frame → AI inference → draw results → show frame.
while True:

    # --- Read one frame from the camera ---
    # ret is True if the frame was read successfully, False if camera failed
    # frame is a NumPy array of shape (height, width, 3) - 3 for BGR channels
    ret, frame = cap.read()

    # If the camera stops working, exit the loop
    if not ret:
        print("Error: Could not read frame from camera")
        break

    # --- Run the AI model on this frame ---
    # model(frame) sends the image through the neural network and returns
    # a list of detection results. verbose=False suppresses debug output.
    # This is where the AI "sees" the image and finds body poses.
    results = model(frame, verbose=False)

    # --- Draw skeletons for each person detected ---
    # results[0].keypoints contains all detected people's joint positions.
    # .xy gives x,y pixel coordinates, .conf gives confidence scores.
    # We loop through each person detected in the frame.
    num_people = len(results[0].keypoints.xy)

    for person_idx in range(num_people):
        # Get this person's keypoint coordinates as a NumPy array (17, 2)
        # .cpu() moves data from GPU to CPU, .numpy() converts to NumPy
        keypoints = results[0].keypoints.xy[person_idx].cpu().numpy()

        # Get confidence scores for each keypoint as array of 17 values
        confidences = results[0].keypoints.conf[person_idx].cpu().numpy()

        # Draw skeleton lines first (so dots appear on top of lines)
        for start_idx, end_idx in SKELETON_CONNECTIONS:
            # Only draw the line if BOTH endpoints have high enough confidence
            if confidences[start_idx] > CONFIDENCE_THRESHOLD and \
               confidences[end_idx] > CONFIDENCE_THRESHOLD:
                # Get the x,y positions and convert to integers (pixels)
                start_point = tuple(keypoints[start_idx].astype(int))
                end_point = tuple(keypoints[end_idx].astype(int))
                # Draw a line between the two keypoints
                cv2.line(frame, start_point, end_point, SKELETON_COLOR, 2)

        # Draw keypoint dots on top of the skeleton lines
        for kp_idx in range(len(keypoints)):
            # Only draw if the AI is confident enough about this keypoint
            if confidences[kp_idx] > CONFIDENCE_THRESHOLD:
                # Convert float coordinates to integer pixel positions
                x, y = int(keypoints[kp_idx][0]), int(keypoints[kp_idx][1])
                # Draw a filled circle at the keypoint position
                # Parameters: image, center, radius, color, thickness (-1 = filled)
                cv2.circle(frame, (x, y), 5, KEYPOINT_COLOR, -1)

    # --- Calculate and display FPS ---
    current_time = time.time()
    # FPS = 1 / (time between frames). Avoid division by zero with max().
    fps = 1.0 / max(current_time - prev_time, 0.001)
    prev_time = current_time

    # Draw FPS text on the frame so you can see performance
    # putText parameters: image, text, position, font, scale, color, thickness
    cv2.putText(frame, f"FPS: {fps:.1f}", (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX, 1.0, TEXT_COLOR, 2)

    # --- Show the frame in a window ---
    # imshow displays the frame in a window titled "Pose Estimation"
    cv2.imshow("Pose Estimation", frame)

    # --- Check if user pressed 'q' to quit ---
    # waitKey(1) waits 1 millisecond for a key press and returns the key code.
    # & 0xFF masks the result to get the actual ASCII value (needed on some OS).
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# ==============================================================================
# Step 5: Cleanup - release resources
# ==============================================================================

# Always release the camera when done - this frees it for other programs
cap.release()

# Close all OpenCV display windows
cv2.destroyAllWindows()

print("\nPose estimation stopped. Goodbye!")

# ==============================================================================
# Reference: Key Concepts Explained
# ==============================================================================

# COCO Keypoint Format:
#   COCO stands for "Common Objects in Context" - a large dataset used to
#   train AI models. The pose format defines 17 body keypoints (listed above
#   in Step 3). This is the standard format used by most pose estimation models.
#
# What is a Neural Network?
#   A neural network is a program inspired by the human brain. It learns
#   patterns from millions of example images. YOLOv8 was trained on the
#   COCO dataset (over 200,000 images with labeled body poses) so it can
#   recognize human poses in images it has never seen before.
#
# What is Inference?
#   "Inference" means running a trained model on new data to get predictions.
#   Training = teaching the model (takes days on powerful GPUs).
#   Inference = using the model (takes milliseconds per frame).
#
# Confidence Scores:
#   For each keypoint, the model outputs a confidence score from 0.0 to 1.0.
#   - 0.9+ = very confident (keypoint clearly visible)
#   - 0.5-0.9 = somewhat confident (partially occluded)
#   - Below 0.5 = unreliable (hidden behind body, out of frame)
#   We filter by CONFIDENCE_THRESHOLD to avoid drawing incorrect keypoints.
#
# BGR vs RGB:
#   Most image formats and libraries use RGB (Red, Green, Blue) order.
#   OpenCV historically uses BGR (Blue, Green, Red) - the channels are reversed.
#   This matters when specifying colors:
#     BGR (0, 0, 255) = Red     (not blue!)
#     BGR (255, 0, 0) = Blue    (not red!)
#     BGR (0, 255, 0) = Green   (same in both)
#   If your colors look wrong, you probably mixed up BGR and RGB.
#
# FPS (Frames Per Second):
#   FPS measures how many frames the program processes each second.
#   - 30+ FPS = smooth real-time video
#   - 15-30 FPS = usable but slightly choppy
#   - Below 15 FPS = noticeably slow
#   FPS depends on your GPU (NVIDIA CUDA is much faster than CPU-only).
#
# GPU vs CPU for AI:
#   - CPU (Central Processing Unit): General-purpose, processes tasks one by one
#   - GPU (Graphics Processing Unit): Specialized for parallel math operations
#   - AI models run 10-50x faster on GPU than CPU
#   - YOLOv8 automatically uses GPU (CUDA) if available, falls back to CPU
#
# Try these experiments:
#   1. Change CONFIDENCE_THRESHOLD to 0.3 - notice more (noisier) detections
#   2. Change model to "yolov8n-pose.pt" - faster but less accurate
#   3. Change KEYPOINT_COLOR to (0, 0, 255) for red dots (remember: BGR!)
#   4. Add cv2.flip(frame, 1) before model() to mirror the image
#   5. Have two people stand in front of the camera - it tracks multiple people
