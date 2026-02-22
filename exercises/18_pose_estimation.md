# Exercise 18: Computer Vision - Pose Estimation

[← Previous: Council Voting](17_council_voting.md) | [Back to Exercises](README.md)

## What You Learned

- Real-time pose estimation with YOLOv8 and a webcam
- The COCO 17-keypoint body model (nose, eyes, shoulders, elbows, wrists, hips, knees, ankles)
- OpenCV basics: camera capture, color spaces, drawing, display windows
- How confidence scores filter unreliable detections
- The difference between BGR and RGB color formats
- Frame-by-frame video processing loop pattern

## Exercise 18.1: Blue Ball Detection

**Concept:** HSV color space separates *color* from *brightness*, making color detection much more reliable than using BGR directly.

- **Hue** (0-179) = position on the color wheel. Blue is around 100-130. Red is 0-10 and 170-179. Green is 40-80.
- **Saturation** (0-255) = how vivid the color is. 0 = gray, 255 = pure color.
- **Value** (0-255) = brightness. 0 = black, 255 = bright.

**Why HSV instead of BGR?** A blue ball in shadow has very different BGR values than the same ball in sunlight. But in HSV, the *Hue* stays roughly the same - only Value (brightness) changes. This makes detection work across different lighting conditions.

**Materials:** Any blue object - a ball, a marker cap, a toy, a blue phone case.

**Task:** Detect a blue object in your webcam feed and draw a circle around it.

```python
import cv2
import numpy as np

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Convert from BGR to HSV color space
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Define range for blue color in HSV
    lower_blue = np.array([100, 50, 50])
    upper_blue = np.array([130, 255, 255])

    # Create a mask: white where blue exists, black everywhere else
    mask = cv2.inRange(hsv, lower_blue, upper_blue)

    # Find contours (outlines) of the white regions in the mask
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if contours:
        # Find the largest blue region
        largest = max(contours, key=cv2.contourArea)
        if cv2.contourArea(largest) > 500:  # Ignore tiny specks
            # Get the minimum enclosing circle
            (x, y), radius = cv2.minEnclosingCircle(largest)
            cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 2)
            cv2.putText(frame, "Blue!", (int(x)-20, int(y)-int(radius)-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

    cv2.imshow("Blue Ball Detector", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

**What to observe:**
- How does lighting affect detection? Try moving the blue object into shadow and bright light.
- Try changing the HSV range for **red** (H: 0-10 *and* 170-179 - red wraps around the hue wheel!) or **green** (H: 40-80).
- What happens when you wear a blue shirt? Does it detect that too?

## Exercise 18.2: Shape Detection

**Concept:** Edges are boundaries where pixel brightness changes sharply. By finding edges and tracing their outlines (contours), we can detect shapes. Counting the number of vertices (corners) tells us what shape it is.

**Printable shapes:** Run `python exercises/18_generate_shapes_pdf.py` to generate `18_shapes.pdf`. Print it and cut out the shapes, or just hold the printed sheet in front of your camera.

**Task:** Hold shapes in front of the camera and classify them by vertex count.

```python
import cv2

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Convert to grayscale (edges work on brightness, not color)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Blur slightly to reduce noise (noise creates false edges)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)

    # Canny edge detection: finds pixels where brightness changes sharply
    edges = cv2.Canny(blurred, 50, 150)

    # Find contours (outlines) from the edges
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    for contour in contours:
        area = cv2.contourArea(contour)
        if area < 1000:  # Skip tiny contours (noise)
            continue

        # Approximate the contour with fewer points
        # epsilon controls how much simplification: smaller = more detail
        perimeter = cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, 0.04 * perimeter, True)
        vertices = len(approx)

        # Classify by vertex count
        if vertices == 3:
            shape = "Triangle"
        elif vertices == 4:
            shape = "Rectangle"
        elif vertices == 5:
            shape = "Pentagon"
        elif vertices > 8:
            shape = "Circle"
        else:
            shape = f"{vertices}-gon"

        # Draw the contour and label
        cv2.drawContours(frame, [approx], -1, (0, 255, 0), 2)
        x, y = approx[0][0]
        cv2.putText(frame, f"{shape} ({vertices})", (x, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

    cv2.imshow("Shape Detector", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

**What to observe:**
- Why does a circle have many vertices? (A circle's contour is a smooth curve approximated by many short line segments.)
- What happens with a crumpled or hand-drawn shape?
- Try adjusting the `0.04` epsilon value. Smaller = more vertices detected, larger = more simplification.

## Exercise 18.3: ArUco Marker Detection

**Concept:** ArUco markers are special black-and-white square patterns that computers can detect extremely reliably. Each marker has a unique binary ID encoded in its grid pattern. They work from steep angles, at a distance, and even when partially hidden - making them useful for robotics, augmented reality, and camera calibration.

**Printable markers:** Run `python exercises/18_generate_aruco_pdf.py` to generate a marker sheet. Print it out.

**Task:** Detect markers, display their IDs, and draw their outlines.

```python
import cv2

# Load the ArUco dictionary (defines which marker patterns to look for)
aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
parameters = cv2.aruco.DetectorParameters()
detector = cv2.aruco.ArucoDetector(aruco_dict, parameters)

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Detect markers in the frame
    corners, ids, rejected = detector.detectMarkers(frame)

    if ids is not None:
        # Draw detected markers with their IDs
        cv2.aruco.drawDetectedMarkers(frame, corners, ids)
        for i, marker_id in enumerate(ids):
            print(f"Detected marker ID: {marker_id[0]}")

    cv2.imshow("ArUco Detector", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

**Extension:** Tape 2 markers to a wall at a known distance apart (e.g., 50 cm). Measure the pixel distance between their centers in the image. Now you can calculate a pixels-per-cm scale for that depth:

```python
pixels_per_cm = pixel_distance / real_distance_cm
```

**What to observe:**
- Tilt the printed sheet at steep angles. At what angle does detection fail?
- Cover part of a marker with your finger. How much can you hide before it stops detecting?
- How far away can you hold the sheet and still get reliable detection?

## Exercise 18.4: Face Detection with Haar Cascades

**Concept:** Before deep learning, computer vision used hand-crafted feature detectors. Haar cascades (Viola-Jones algorithm, published in 2001) scan a sliding window across the image looking for patterns of light and dark regions that match faces. No model download needed - Haar cascades ship built into OpenCV.

**Task:** Detect faces and draw rectangles around them.

```python
import cv2

# Load the pre-trained face cascade (ships with OpenCV, no download needed)
face_cascade = cv2.CascadeClassifier(
    cv2.data.haarcascades + "haarcascade_frontalface_default.xml"
)

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect faces: scaleFactor shrinks image iteratively, minNeighbors filters noise
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.3, minNeighbors=5)

    for (x, y, w, h) in faces:
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cv2.putText(frame, "Face", (x, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

    cv2.imshow("Haar Face Detector", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

**Comparison challenge:** Run this Haar cascade detector AND the YOLO pose estimator (Example 18) side by side. Which is faster? Which is more accurate? Haar cascades were state-of-the-art in 2001. YOLO represents 2024 deep learning. That is 23 years of progress.

**What to observe:**
- Turn your head to the side (profile view). Does it still detect your face?
- Put on sunglasses. Does detection fail?
- Cover your mouth with your hand or a mask. What happens?
- Have multiple people in the frame. Does it find everyone?

## Exercise 18.5: Skeleton Analysis - Knee Angle

**Concept:** With keypoint positions, we can measure angles between body parts using trigonometry (`atan2`). This is the foundation of biomechanics analysis - understanding how the body moves.

**Task:** Modify the main pose estimation example (Example 18) to calculate and display the real-time knee angle.

Here is the angle calculation function:

```python
import math

def angle_between(a, b, c):
    """Calculate the angle at point b formed by points a-b-c.
    a, b, c are (x, y) tuples. b is the vertex (corner) of the angle."""
    ba = (a[0] - b[0], a[1] - b[1])
    bc = (c[0] - b[0], c[1] - b[1])
    dot = ba[0] * bc[0] + ba[1] * bc[1]
    mag_ba = math.sqrt(ba[0]**2 + ba[1]**2)
    mag_bc = math.sqrt(bc[0]**2 + bc[1]**2)
    if mag_ba * mag_bc == 0:
        return 0
    cos_angle = max(-1, min(1, dot / (mag_ba * mag_bc)))
    return math.degrees(math.acos(cos_angle))
```

Use the COCO keypoints for the left leg: **hip (11)** as point a, **knee (13)** as point b (vertex), **ankle (15)** as point c. Add this inside the person loop in Example 18:

```python
# After getting keypoints and confidences for a person:
if confidences[11] > 0.5 and confidences[13] > 0.5 and confidences[15] > 0.5:
    hip = tuple(keypoints[11])
    knee = tuple(keypoints[13])
    ankle = tuple(keypoints[15])
    knee_angle = angle_between(hip, knee, ankle)
    cv2.putText(frame, f"Knee: {knee_angle:.0f} deg",
                (int(knee[0]) + 10, int(knee[1])),
                cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
```

**Extension - Squat counter:** Count squats! When knee angle drops below 90 degrees and then comes back above 150 degrees, that counts as one squat. Display the count on screen.

```python
squat_count = 0
in_squat = False

# Inside the loop, after calculating knee_angle:
if knee_angle < 90:
    in_squat = True
if in_squat and knee_angle > 150:
    squat_count += 1
    in_squat = False
cv2.putText(frame, f"Squats: {squat_count}", (10, 70),
            cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 255, 255), 2)
```

**What to observe:**
- Stand up straight: knee angle should be around 170 degrees (not exactly 180 because legs are never perfectly straight).
- Do a deep squat: knee angle drops to around 60 degrees.
- Try lunges, jumps, or sitting down. How does the angle change?

## Exercise 18.6: Build Your Own - Mini Jump Analyzer

**Concept:** By tracking keypoint positions over time (frame by frame), we can detect events like jumps, measure air time, and analyze movement patterns. This combines time-series thinking with pose estimation.

**Task:** Track hip Y position over time, detect when someone jumps, and measure how long they are in the air.

**Hints** (no full solution - build it yourself!):

1. **Store hip Y values:** Keep a list of hip Y positions, one per frame. Remember that in image coordinates, Y increases *downward* - so a jump makes hip Y *decrease*.

2. **Detect "in air":** Compare current hip Y to a running average of recent frames. If hip Y is significantly above (lower pixel value) the average, the person is in the air.

   ```python
   hip_y_history = []
   # Each frame, append the current hip Y:
   hip_y_history.append(hip_y)
   # Running average of last 30 frames:
   avg = sum(hip_y_history[-30:]) / min(len(hip_y_history), 30)
   # "In air" if hip is more than 50 pixels above average:
   in_air = hip_y < (avg - 50)
   ```

3. **Measure air time:** Count how many consecutive frames the person is "in air", then divide by FPS.

   ```python
   air_time_seconds = frames_in_air / fps
   ```

4. **Visualize:** Draw a simple bar chart of hip Y over the last 60 frames as a horizontal strip at the bottom of the screen.

5. **Reference:** The [jump](https://github.com/patdeg/jump) project does this at production scale with phase detection (approach, loading, takeoff, flight, landing), coaching flags (chicken wing, stiff landing, incomplete extension), and full biomechanics metrics. Check it out to see where this exercise can lead.

**What to observe:**
- How many pixels does hip Y change during a normal standing jump?
- How about during a volleyball spike approach with a run-up?
- What is a typical air time for a standing jump versus a running jump?

## Reflection Questions

After completing these exercises, consider:

1. What is the difference between classical computer vision (Haar cascades, edge detection) and deep learning (YOLO)? When might you prefer the simpler approach?
2. Why does HSV work better than BGR for color detection? Think about what happens to BGR values when lighting changes.
3. How does the confidence score help filter bad detections? What happens if you set the threshold too low or too high?
4. What happens to skeleton accuracy when you stand far from the camera versus close up? Why?
5. Could you combine ArUco markers with pose estimation? For example, define a "jump zone" on the floor with markers and measure if the athlete stays within it.
6. What other sports or activities could benefit from pose estimation? Think about form correction, injury prevention, or performance tracking.

## Next Steps

These exercises covered the fundamentals of computer vision and pose estimation. To go further:

- Explore different YOLO model sizes (`yolov8n-pose` for speed, `yolov8x-pose` for accuracy)
- Try tracking multiple people simultaneously
- Combine pose estimation with the audio tools from earlier examples (voice-activated recording)
- Look into 3D pose estimation using stereo cameras or depth sensors

---

**Related Examples:**
- [python/18_pose_estimation.py](../python/18_pose_estimation.py)

[← Previous: Council Voting](17_council_voting.md) | [Back to Exercises](README.md)
