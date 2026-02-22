# Example 18: Pose Estimation - Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add Example 18 (Computer Vision - Pose Estimation) to ai101 with Python (GPU/CPU + NPU), Go (GoCV), exercises, and printable PDFs.

**Architecture:** Single self-contained file per language variant (Approach A). Python uses ultralytics for GPU/CPU, CIX NOE_Engine for NPU. Go uses GoCV + ONNX. Exercises progress from color detection to skeleton analysis with printable materials.

**Tech Stack:** Python (ultralytics, opencv-python, numpy), Go (GoCV, gocv.io/x/gocv), PDF generation (matplotlib, cv2)

---

### Task 1: Update .gitignore and create models directory

**Files:**
- Modify: `/mnt/e/Dev/ai101/.gitignore` (append after line 568, before `# END OF .gitignore`)

**Step 1: Add models/ to .gitignore**

Add before the `# END OF .gitignore` comment:

```
# AI models (downloaded on first run)
models/
```

**Step 2: Verify**

Run: `grep -n "models/" /mnt/e/Dev/ai101/.gitignore`
Expected: Line with `models/`

**Step 3: Commit**

```bash
cd /mnt/e/Dev/ai101
git add .gitignore
git commit -m "chore: add models/ to gitignore for Example 18"
```

---

### Task 2: Python main example (ultralytics GPU/CPU)

**Files:**
- Create: `/mnt/e/Dev/ai101/python/18_pose_estimation.py`

**Step 1: Write the example**

Create `python/18_pose_estimation.py` following the exact ai101 pattern:
- Shebang + docstring header (WHAT THIS DEMONSTRATES, WHAT YOU'LL LEARN, PREREQUISITES, EXPECTED OUTPUT)
- Step-by-step numbered sections with `# ===` separators
- 80% comments, ~200 lines total
- Reference section at bottom

The code structure:

```python
#!/usr/bin/env python3
"""
Example 18: Computer Vision - Real-Time Pose Estimation

WHAT THIS DEMONSTRATES:
    - Real-time computer vision using a webcam
    - AI pose estimation with YOLOv8s-pose (detects 17 body keypoints)
    - Drawing skeleton overlays on live video frames
    - Understanding how AI "sees" the human body

WHAT YOU'LL LEARN:
    - Video capture with OpenCV (reading frames from a camera)
    - Running an AI model on each frame (inference)
    - Keypoint detection: nose, eyes, shoulders, elbows, wrists, hips, knees, ankles
    - Skeleton connections: which keypoints to connect with lines
    - Confidence scores: how sure the AI is about each detection
    - Real-time processing loop (capture -> detect -> draw -> display -> repeat)

PREREQUISITES:
    - Python 3.8 or higher
    - A webcam (built-in or USB)
    - Install dependencies:
        pip install ultralytics opencv-python numpy
    - First run downloads the YOLOv8s-pose model (~12 MB)

EXPECTED OUTPUT:
    - A window showing your camera feed with a 17-point skeleton overlay
    - Colored lines connecting body parts (limbs)
    - Dots on each detected keypoint (joints)
    - FPS counter showing processing speed
    - Press 'q' to quit

Run with: python3 18_pose_estimation.py

Hardware note:
    - Works on any computer with a webcam
    - GPU (NVIDIA CUDA) makes it faster but is NOT required
    - For Orange Pi 5 Plus with NPU, see 18_pose_estimation_npu.py
"""

import cv2    # OpenCV: camera capture, image processing, drawing, display
import time   # For measuring FPS (frames per second)

# ==============================================================================
# Step 1: Load the AI model
# ==============================================================================

# What is a "model"?
# A model is a file containing millions of numbers (called "weights") that were
# learned by training on thousands of images of people. When you show it a new
# image, it uses those weights to predict where body parts are.
#
# YOLOv8s-pose is a specific model that:
#   - "YOLO" = You Only Look Once (processes the whole image in one pass)
#   - "v8" = version 8 of the YOLO family
#   - "s" = small size (fast, good accuracy)
#   - "pose" = trained to detect human body keypoints
#
# The first time you run this, ultralytics will download the model (~12 MB).
# After that, it loads from your local cache.

from ultralytics import YOLO

print("Loading YOLOv8s-pose model...")
model = YOLO("yolov8s-pose.pt")  # Downloads automatically on first run
print("Model loaded!")

# ==============================================================================
# Step 2: Open the camera
# ==============================================================================

# cv2.VideoCapture(0) opens the default camera (index 0).
# If you have multiple cameras, try 1, 2, etc.
# On Orange Pi: USB cameras are usually at index 0 or 1.

camera = cv2.VideoCapture(0)

# Check if the camera opened successfully
if not camera.isOpened():
    print("Error: Could not open camera!")
    print("Try a different camera index: cv2.VideoCapture(1)")
    exit(1)

# Read camera properties
frame_width = int(camera.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(camera.get(cv2.CAP_PROP_FRAME_HEIGHT))
print(f"Camera opened: {frame_width}x{frame_height}")

# ==============================================================================
# Step 3: Define the skeleton connections
# ==============================================================================

# YOLOv8-pose detects 17 keypoints in this order (COCO format):
#
#    0: Nose          1: Left Eye       2: Right Eye
#    3: Left Ear      4: Right Ear
#    5: Left Shoulder  6: Right Shoulder
#    7: Left Elbow     8: Right Elbow
#    9: Left Wrist    10: Right Wrist
#   11: Left Hip      12: Right Hip
#   13: Left Knee     14: Right Knee
#   15: Left Ankle    16: Right Ankle
#
# To draw a skeleton, we connect keypoints with lines.
# Each pair below defines one "bone" of the skeleton.

SKELETON_CONNECTIONS = [
    (0, 1), (0, 2),     # Nose to eyes
    (1, 3), (2, 4),     # Eyes to ears
    (5, 6),             # Shoulder to shoulder
    (5, 7), (7, 9),     # Left arm: shoulder -> elbow -> wrist
    (6, 8), (8, 10),    # Right arm: shoulder -> elbow -> wrist
    (5, 11), (6, 12),   # Shoulders to hips
    (11, 12),            # Hip to hip
    (11, 13), (13, 15),  # Left leg: hip -> knee -> ankle
    (12, 14), (14, 16),  # Right leg: hip -> knee -> ankle
]

# Colors for drawing (BGR format - Blue, Green, Red)
# OpenCV uses BGR, not RGB! So (255, 0, 0) is blue, not red.
KEYPOINT_COLOR = (0, 255, 0)    # Green dots for keypoints
SKELETON_COLOR = (0, 255, 255)  # Yellow lines for bones
TEXT_COLOR = (255, 255, 255)    # White text

# ==============================================================================
# Step 4: The main loop - capture, detect, draw, display
# ==============================================================================

# This is the heart of any computer vision application:
#   1. Read one frame from the camera (a single image)
#   2. Run the AI model on that frame (detect keypoints)
#   3. Draw the results on the frame (skeleton overlay)
#   4. Show the frame on screen
#   5. Repeat!
#
# A "frame" is just a picture. Video is ~30 pictures per second.
# The faster we can process each frame, the smoother the video looks.

print("\nStarting pose estimation... Press 'q' to quit.\n")

frame_count = 0
start_time = time.time()

while True:
    # ---- Step 4a: Capture one frame from the camera ----
    # camera.read() returns two things:
    #   - success: True if we got a frame, False if camera disconnected
    #   - frame: the image as a numpy array of shape (height, width, 3)
    #            The 3 channels are Blue, Green, Red (BGR)
    success, frame = camera.read()
    if not success:
        print("Error: Could not read frame from camera")
        break

    # ---- Step 4b: Run pose estimation on this frame ----
    # model() runs the AI inference:
    #   - Resizes the image to 640x640 (the model's expected input)
    #   - Passes it through the neural network
    #   - Returns detected people with their keypoints
    #
    # verbose=False prevents printing detection details every frame
    results = model(frame, verbose=False)

    # ---- Step 4c: Draw the skeleton on the frame ----
    # results[0] contains the detections for our single image.
    # results[0].keypoints contains the body keypoints.
    #
    # For each person detected:
    #   - keypoints.xy gives (x, y) pixel coordinates for each keypoint
    #   - keypoints.conf gives the confidence (0.0 to 1.0) for each
    #
    # We only draw keypoints with confidence > 0.5 (50% sure)

    if results[0].keypoints is not None:
        keypoints_data = results[0].keypoints

        # Loop through each person detected
        for person_idx in range(len(keypoints_data)):
            # Get this person's keypoint coordinates and confidence scores
            kpts_xy = keypoints_data.xy[person_idx].cpu().numpy()    # shape: (17, 2)
            kpts_conf = keypoints_data.conf[person_idx].cpu().numpy() # shape: (17,)

            # Draw skeleton connections (lines between keypoints)
            for (start_idx, end_idx) in SKELETON_CONNECTIONS:
                # Only draw if BOTH keypoints are confident enough
                if kpts_conf[start_idx] > 0.5 and kpts_conf[end_idx] > 0.5:
                    # Get pixel coordinates (convert float to int for drawing)
                    x1, y1 = int(kpts_xy[start_idx][0]), int(kpts_xy[start_idx][1])
                    x2, y2 = int(kpts_xy[end_idx][0]), int(kpts_xy[end_idx][1])

                    # Draw a line from start keypoint to end keypoint
                    cv2.line(frame, (x1, y1), (x2, y2), SKELETON_COLOR, 2)

            # Draw keypoint dots
            for kpt_idx in range(17):
                if kpts_conf[kpt_idx] > 0.5:
                    x, y = int(kpts_xy[kpt_idx][0]), int(kpts_xy[kpt_idx][1])
                    # Draw a filled circle at each keypoint
                    cv2.circle(frame, (x, y), 5, KEYPOINT_COLOR, -1)

    # ---- Step 4d: Calculate and display FPS ----
    frame_count += 1
    elapsed = time.time() - start_time
    fps = frame_count / elapsed if elapsed > 0 else 0

    # Draw FPS text on the frame
    cv2.putText(
        frame,
        f"FPS: {fps:.1f}",
        (10, 30),                        # Position (x, y)
        cv2.FONT_HERSHEY_SIMPLEX,        # Font
        1.0,                             # Font scale
        TEXT_COLOR,                       # Color
        2,                               # Thickness
    )

    # ---- Step 4e: Display the frame in a window ----
    cv2.imshow("Pose Estimation - Press 'q' to quit", frame)

    # ---- Step 4f: Check for quit key ----
    # cv2.waitKey(1) waits 1 millisecond for a key press.
    # If the user presses 'q', we break out of the loop.
    # The & 0xFF handles platform differences in key codes.
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# ==============================================================================
# Step 5: Cleanup - release resources
# ==============================================================================

# Always release the camera and close windows when done!
# If you don't, the camera may stay locked and other programs can't use it.
camera.release()
cv2.destroyAllWindows()

print(f"\nDone! Processed {frame_count} frames in {elapsed:.1f}s ({fps:.1f} FPS)")

# ==============================================================================
# Reference: COCO 17 Keypoints
# ==============================================================================
#
# The 17 keypoints detected by YOLOv8-pose follow the COCO (Common Objects in
# Context) format. This is a standard used across many pose estimation models.
#
# Index  Name            Body Part
# -----  --------------  ---------
#   0    nose            Head
#   1    left_eye        Head
#   2    right_eye       Head
#   3    left_ear        Head
#   4    right_ear       Head
#   5    left_shoulder   Upper body
#   6    right_shoulder  Upper body
#   7    left_elbow      Left arm
#   8    right_elbow     Right arm
#   9    left_wrist      Left arm
#  10    right_wrist     Right arm
#  11    left_hip        Lower body
#  12    right_hip       Lower body
#  13    left_knee       Left leg
#  14    right_knee      Right leg
#  15    left_ankle      Left leg
#  16    right_ankle     Right leg
#
# Why 17? This set covers all major joints needed to understand human pose.
# It's enough to analyze walking, running, jumping, dancing, exercises, etc.
#
# Keypoint coordinate format:
#   Each keypoint has (x, y, confidence):
#   - x: horizontal position in pixels (0 = left edge)
#   - y: vertical position in pixels (0 = top edge)
#   - confidence: 0.0 to 1.0 (how sure the model is)
#
# What is a neural network?
#   Think of it as a function with millions of adjustable knobs (parameters).
#   During training, the knobs are tuned by showing the network thousands of
#   images where humans manually labeled the keypoint positions. After training,
#   the network can predict keypoints on images it has never seen before.
#
# What is inference?
#   "Inference" means using a trained model to make predictions on new data.
#   Training takes hours/days on powerful GPUs. Inference takes milliseconds.
#
# What is FPS?
#   Frames Per Second - how many images the system processes each second.
#   30 FPS = smooth video. 10-15 FPS = usable but choppy. <5 FPS = slow.
#   On GPU (NVIDIA): 30-60+ FPS. On CPU: 5-15 FPS. On NPU: 15-30 FPS.
#
# Try these modifications:
#   - Change camera index: cv2.VideoCapture(1) for a second camera
#   - Change confidence threshold: try 0.3 (more detections) or 0.8 (fewer)
#   - Change skeleton colors: try (0, 0, 255) for red, (255, 0, 0) for blue
#   - Add keypoint labels: draw the keypoint name next to each dot
#   - Mirror the image: frame = cv2.flip(frame, 1) for selfie mode
```

**Step 2: Test the example**

Run: `cd /mnt/e/Dev/ai101 && source ../jump/venv/bin/activate && python3 python/18_pose_estimation.py`
Expected: Camera window opens with skeleton overlay. Press 'q' to quit.

Note: If no display (headless/WSL), verify no import errors:
Run: `python3 -c "from ultralytics import YOLO; import cv2; print('OK')"`

**Step 3: Commit**

```bash
cd /mnt/e/Dev/ai101
git add python/18_pose_estimation.py
git commit -m "feat: add Example 18 - pose estimation (Python, ultralytics)"
```

---

### Task 3: Python NPU variant (Orange Pi 5 Plus)

**Files:**
- Create: `/mnt/e/Dev/ai101/python/18_pose_estimation_npu.py`

**Step 1: Write the NPU variant**

Same structure as Task 2 but:
- Header explains NPU: "a dedicated AI chip on the Orange Pi board"
- Imports NOE_Engine instead of ultralytics
- Manual preprocessing (letterbox resize, normalize, NCHW)
- Manual output parsing (NMS, keypoint extraction)
- Reference the CIX NPU code from `/mnt/e/Dev/jump/inference/cix_npu.py` for the preprocessing, NMS, and keypoint extraction patterns
- Same skeleton drawing and display loop

The key differences to document in comments:
- Model file is `.cix` format (pre-compiled for NPU), not `.pt`
- Preprocessing must be done manually (ultralytics does it automatically)
- Output tensor must be parsed manually (bounding boxes + keypoints)
- NMS (Non-Maximum Suppression) removes duplicate detections

Model path: `/opt/ai/ai_model_hub_25_Q3/models/ComputeVision/Pose_Estimation/onnx_yolov8s_pose/yolov8s-pose.cix`

**Step 2: Commit**

```bash
cd /mnt/e/Dev/ai101
git add python/18_pose_estimation_npu.py
git commit -m "feat: add Example 18 NPU variant for Orange Pi 5 Plus"
```

---

### Task 4: Go version with GoCV

**Files:**
- Create: `/mnt/e/Dev/ai101/go/18_pose_estimation.go`

**Step 1: Write the Go example**

Follows ai101 Go style:
- Package main, all imports listed
- Struct definitions for Keypoint and Person
- Step-by-step comments
- Uses `gocv.VideoCaptureDevice(0)` for camera
- Uses `gocv.ReadNetFromONNX("yolov8s-pose.onnx")` for model
- BlobFromImage for preprocessing
- net.Forward() for inference
- Manual output parsing (Go doesn't have ultralytics)
- Same skeleton drawing with `gocv.Line` and `gocv.Circle`

Prerequisites documented in header:
- GoCV installation: `go get -u -d gocv.io/x/gocv && cd $GOPATH/pkg/mod/gocv.io/x/gocv* && make install`
- ONNX model file: export from ultralytics or download

**Step 2: Commit**

```bash
cd /mnt/e/Dev/ai101
git add go/18_pose_estimation.go
git commit -m "feat: add Example 18 - pose estimation (Go, GoCV)"
```

---

### Task 5: Shape PDF generator

**Files:**
- Create: `/mnt/e/Dev/ai101/exercises/18_generate_shapes_pdf.py`

**Step 1: Write the generator script**

~80 lines. Uses OpenCV + matplotlib to draw shapes on a white page:

- Page title: "Shape Detection Test Sheet - Cut along dashed lines"
- 8 shapes in a 2x4 grid, each ~5cm:
  - Circle (labeled "CIRCLE")
  - Equilateral triangle (labeled "TRIANGLE")
  - Square (labeled "SQUARE")
  - Rectangle (labeled "RECTANGLE")
  - Pentagon (labeled "PENTAGON")
  - Hexagon (labeled "HEXAGON")
  - Star (5-pointed, labeled "STAR")
  - Arrow (labeled "ARROW")
- Each shape drawn with thick black outline on white background
- Dashed cut lines around each shape
- Labels below each shape
- Outputs `exercises/18_shapes.pdf`
- Educational header explaining what each shape teaches about contour detection

**Step 2: Generate the PDF**

Run: `cd /mnt/e/Dev/ai101 && python3 exercises/18_generate_shapes_pdf.py`
Expected: Creates `exercises/18_shapes.pdf`

**Step 3: Commit**

```bash
cd /mnt/e/Dev/ai101
git add exercises/18_generate_shapes_pdf.py exercises/18_shapes.pdf
git commit -m "feat: add shape detection PDF generator for Exercise 18.2"
```

---

### Task 6: ArUco marker PDF generator

**Files:**
- Create: `/mnt/e/Dev/ai101/exercises/18_generate_aruco_pdf.py`

**Step 1: Write the generator script**

~60 lines. Uses cv2.aruco + matplotlib:

- Generates 12 ArUco markers (4x3 grid) from DICT_4X4_50
- Each marker ~6cm with ID label below
- Page title: "ArUco Marker Sheet - Print at 100% scale"
- Outputs `exercises/18_aruco_markers.pdf`
- Educational comments explaining:
  - What ArUco markers are (like QR codes but simpler, designed for CV)
  - Why 4x4 dictionary (fewer bits = readable from farther away)
  - How IDs work (each marker has a unique binary pattern)

**Step 2: Generate the PDF**

Run: `cd /mnt/e/Dev/ai101 && python3 exercises/18_generate_aruco_pdf.py`
Expected: Creates `exercises/18_aruco_markers.pdf`

**Step 3: Commit**

```bash
cd /mnt/e/Dev/ai101
git add exercises/18_generate_aruco_pdf.py exercises/18_aruco_markers.pdf
git commit -m "feat: add ArUco marker PDF generator for Exercise 18.3"
```

---

### Task 7: Exercise file

**Files:**
- Create: `/mnt/e/Dev/ai101/exercises/18_pose_estimation.md`

**Step 1: Write the exercise**

Follow existing exercise format:
- Navigation links (Previous: Council Voting | Back to Exercises | no Next)
- "What You Learned" section
- 6 exercises (18.1 through 18.6) with progressive difficulty
- Each exercise has: concept explanation, task, code snippet, "what to observe"
- Reflection questions
- Related examples links

Exercise content (from design doc):
- 18.1: Blue Ball Detection (HSV color space, contour detection)
- 18.2: Shape Detection (Canny edges, approxPolyDP, printable PDF)
- 18.3: ArUco Marker Detection (fiducial markers, printable PDF)
- 18.4: Face Detection (Haar cascades, classical CV)
- 18.5: Skeleton Analysis (angles, squat counter)
- 18.6: Build Your Own - Mini Jump Analyzer (capstone)

**Step 2: Commit**

```bash
cd /mnt/e/Dev/ai101
git add exercises/18_pose_estimation.md
git commit -m "feat: add Exercise 18 - progressive CV exercises with printables"
```

---

### Task 8: Update READMEs and CLAUDE.md

**Files:**
- Modify: `/mnt/e/Dev/ai101/README.md` - Add example 18 to "The 16 Examples" list (now 18), update heading
- Modify: `/mnt/e/Dev/ai101/python/README.md` - Add example 18 entry and note about pip install
- Modify: `/mnt/e/Dev/ai101/go/README.md` - Add example 18 entry and GoCV setup note
- Modify: `/mnt/e/Dev/ai101/exercises/README.md` - Add row 18 to exercise table
- Modify: `/mnt/e/Dev/ai101/CLAUDE.md` - Update example count, add note about CV deps

**Step 1: Update main README.md**

In the "The 16 Examples" section (line 62), change heading to "The 18 Examples" and add after line 79:

```markdown
17. **Council Voting** - Multi-agent LLM voting and consensus
18. **Pose Estimation** - Real-time skeleton detection with YOLOv8 (webcam + OpenCV)
```

**Step 2: Update python/README.md**

Add after the last example in the run section:

```markdown
# Example 18: Pose Estimation (requires pip install)
pip install ultralytics opencv-python numpy
python3 18_pose_estimation.py          # GPU/CPU (ultralytics)
python3 18_pose_estimation_npu.py      # Orange Pi NPU (CIX)
```

Add exercise link:
```markdown
- **[Exercise 18: Pose Estimation](../exercises/18_pose_estimation.md)** - Color detection, shapes, markers, skeleton analysis
```

**Step 3: Update go/README.md**

Add after the last example:

```markdown
# Example 18: Pose Estimation (requires GoCV)
# Install GoCV first: go get -u -d gocv.io/x/gocv
go run 18_pose_estimation.go
```

**Step 4: Update exercises/README.md**

Add row to table:

```markdown
| 18 | [Pose Estimation](18_pose_estimation.md) | Computer vision, skeleton detection | Color, shapes, markers, skeleton analysis |
```

Update previous exercise (17) navigation link to point to 18 as next.

**Step 5: Update CLAUDE.md**

- Change "16 examples" to "18 examples" where it appears
- Add to the example list: `17. **17_council_voting**` and `18. **18_pose_estimation**`
- Add note: "Example 18 requires `pip install ultralytics opencv-python` (first example with external Python deps)"
- Update exercise count

**Step 6: Commit**

```bash
cd /mnt/e/Dev/ai101
git add README.md python/README.md go/README.md exercises/README.md CLAUDE.md
git commit -m "docs: update READMEs and CLAUDE.md for Example 18"
```

---

### Task 9: Final verification

**Step 1: Verify all files exist**

```bash
ls -la /mnt/e/Dev/ai101/python/18_pose_estimation.py
ls -la /mnt/e/Dev/ai101/python/18_pose_estimation_npu.py
ls -la /mnt/e/Dev/ai101/go/18_pose_estimation.go
ls -la /mnt/e/Dev/ai101/exercises/18_pose_estimation.md
ls -la /mnt/e/Dev/ai101/exercises/18_generate_shapes_pdf.py
ls -la /mnt/e/Dev/ai101/exercises/18_generate_aruco_pdf.py
ls -la /mnt/e/Dev/ai101/exercises/18_shapes.pdf
ls -la /mnt/e/Dev/ai101/exercises/18_aruco_markers.pdf
```

**Step 2: Verify Python example runs (import check)**

```bash
cd /mnt/e/Dev/ai101
python3 -c "
import ast, sys
for f in ['python/18_pose_estimation.py', 'python/18_pose_estimation_npu.py']:
    try:
        ast.parse(open(f).read())
        print(f'{f}: syntax OK')
    except SyntaxError as e:
        print(f'{f}: SYNTAX ERROR: {e}')
        sys.exit(1)
"
```

**Step 3: Verify Go example compiles (syntax check)**

```bash
cd /mnt/e/Dev/ai101/go
go vet 18_pose_estimation.go 2>&1 || echo "Note: go vet may fail without GoCV installed - that is OK"
```

**Step 4: Verify PDF generators run**

```bash
cd /mnt/e/Dev/ai101
python3 exercises/18_generate_shapes_pdf.py
python3 exercises/18_generate_aruco_pdf.py
ls -la exercises/18_shapes.pdf exercises/18_aruco_markers.pdf
```

**Step 5: Verify git status is clean**

```bash
cd /mnt/e/Dev/ai101
git status
git log --oneline -5
```
