package main

// Example 18: Computer Vision - Real-Time Pose Estimation
//
// WHAT THIS DEMONSTRATES:
//   - Real-time computer vision using a webcam in Go
//   - AI pose estimation with YOLOv8s-pose via ONNX model
//   - Drawing skeleton overlays using GoCV (OpenCV bindings for Go)
//
// WHAT YOU'LL LEARN:
//   - GoCV: OpenCV bindings for Go (camera, drawing, display)
//   - ONNX model inference with gocv.ReadNetFromONNX
//   - Blob preprocessing (image -> neural network input)
//   - Parsing neural network output tensors
//   - Real-time processing loops in Go
//
// PREREQUISITES:
//   - Go 1.16 or higher
//   - GoCV installed: go get -u -d gocv.io/x/gocv
//   - OpenCV 4 system library
//   - YOLOv8s-pose ONNX model file (yolov8s-pose.onnx)
//     Export from Python: yolo export model=yolov8s-pose.pt format=onnx
//
// EXPECTED OUTPUT:
//   - Camera window with 17-point skeleton overlay
//   - FPS counter in the top-left corner
//   - Press ESC to quit
//
// Run with: go run 18_pose_estimation.go

import (
	"fmt"
	"image"
	"image/color"
	"math"
	"time"

	"gocv.io/x/gocv"
)

// ==============================================================================
// Step 1: Define constants
// ==============================================================================

// ModelPath is the path to the YOLOv8s-pose ONNX model file.
// ONNX (Open Neural Network Exchange) is a universal format that lets you
// use models trained in Python (PyTorch) from any language, including Go.
//
// To get this file, run in Python:
//   from ultralytics import YOLO
//   model = YOLO("yolov8s-pose.pt")
//   model.export(format="onnx")
const ModelPath = "yolov8s-pose.onnx"

// ModelInputSize is the square image size the neural network expects.
// YOLOv8 models are trained on 640x640 images. We resize every camera frame
// to this size before feeding it to the model, then scale results back.
const ModelInputSize = 640

// ConfidenceThreshold is the minimum confidence score to accept a detection.
// The model outputs a confidence value (0.0 to 1.0) for each detection.
// 0.5 = at least 50% confident. Lower values show more detections but with
// more false positives. Higher values show fewer but more reliable results.
const ConfidenceThreshold = 0.5

// KeypointConfidenceThreshold is the minimum confidence to draw a keypoint.
// Each of the 17 body joints has its own confidence score.
const KeypointConfidenceThreshold = 0.5

// MaxDetections limits how many people we process per frame.
// Processing fewer people = faster. Most scenes have 1-3 people.
const MaxDetections = 10

// COCO 17-keypoint names - these are the body parts the model detects.
// COCO stands for "Common Objects in Context", a large dataset used to
// train AI models. This is the standard body model used worldwide.
//
//	Index   Body Part           Index   Body Part
//	-----   ---------           -----   ---------
//	  0     Nose                  9     Left wrist
//	  1     Left eye             10     Right wrist
//	  2     Right eye            11     Left hip
//	  3     Left ear             12     Right hip
//	  4     Right ear            13     Left knee
//	  5     Left shoulder        14     Right knee
//	  6     Right shoulder       15     Left ankle
//	  7     Left elbow           16     Right ankle
//	  8     Right elbow
//
// Note: "Left" and "Right" are from the PERSON's perspective, not yours.
var keypointNames = [17]string{
	"nose",
	"left_eye", "right_eye",
	"left_ear", "right_ear",
	"left_shoulder", "right_shoulder",
	"left_elbow", "right_elbow",
	"left_wrist", "right_wrist",
	"left_hip", "right_hip",
	"left_knee", "right_knee",
	"left_ankle", "right_ankle",
}

// ==============================================================================
// Step 2: Define skeleton connections (which joints connect to which)
// ==============================================================================

// Each pair is [start_index, end_index] referring to the keypoint indices above.
// For example, [5, 7] connects left shoulder (5) to left elbow (7).
// These connections form the "skeleton" lines drawn between body joints.
var skeletonConnections = [][2]int{
	// Face connections
	{0, 1}, // Nose -> Left eye
	{0, 2}, // Nose -> Right eye
	{1, 3}, // Left eye -> Left ear
	{2, 4}, // Right eye -> Right ear

	// Upper body - left side
	{5, 7}, // Left shoulder -> Left elbow
	{7, 9}, // Left elbow -> Left wrist

	// Upper body - right side
	{6, 8},  // Right shoulder -> Right elbow
	{8, 10}, // Right elbow -> Right wrist

	// Torso (the body's core frame)
	{5, 6},   // Left shoulder -> Right shoulder
	{5, 11},  // Left shoulder -> Left hip
	{6, 12},  // Right shoulder -> Right hip
	{11, 12}, // Left hip -> Right hip

	// Lower body - left side
	{11, 13}, // Left hip -> Left knee
	{13, 15}, // Left knee -> Left ankle

	// Lower body - right side
	{12, 14}, // Right hip -> Right knee
	{14, 16}, // Right knee -> Right ankle
}

// Colors for drawing. GoCV uses BGR (Blue, Green, Red) format, which is
// the opposite of the usual RGB. This is a historical quirk of OpenCV.
// So color.RGBA{R: 0, G: 255, B: 0} is green, but when OpenCV draws it,
// it reads the bytes as BGR. We use gocv's Scalar type or color.RGBA.
var (
	// Green dots for body joints
	keypointColor = color.RGBA{R: 0, G: 255, B: 0, A: 255}

	// Cyan lines for skeleton bones (Blue + Green = Cyan)
	skeletonColor = color.RGBA{R: 255, G: 255, B: 0, A: 255}

	// Yellow text for the FPS counter
	textColor = color.RGBA{R: 0, G: 255, B: 255, A: 255}
)

// ==============================================================================
// Step 3: Define data structures
// ==============================================================================

// Keypoint represents a single detected body joint.
// The model outputs 17 of these per person (nose, eyes, shoulders, etc.)
type Keypoint struct {
	X          float32 // Horizontal position in pixels
	Y          float32 // Vertical position in pixels
	Confidence float32 // How sure the model is (0.0 to 1.0)
}

// Person represents one detected person with their 17 body keypoints.
// The model can detect multiple people in a single frame.
type Person struct {
	Keypoints  [17]Keypoint // COCO 17 keypoints (one per body joint)
	Confidence float32      // Overall detection confidence for this person
	BBox       [4]float32   // Bounding box: x1, y1, x2, y2 (top-left and bottom-right corners)
}

// ==============================================================================
// Step 4: Main function
// ==============================================================================

// MAIN FUNCTION OVERVIEW:
// =======================
// What this example demonstrates:
//   - Opening a webcam with GoCV (OpenCV for Go)
//   - Loading an ONNX neural network model
//   - Real-time inference loop: capture -> preprocess -> detect -> draw -> display
//   - Resource cleanup with Go's defer keyword
//
// What you'll learn:
//   - How GoCV wraps OpenCV's C++ API for Go
//   - How to preprocess images for neural networks (BlobFromImage)
//   - How to parse raw tensor output from a neural network
//   - Go's defer pattern for guaranteed cleanup
//
// Expected output:
//   - A live camera window with skeleton overlays on detected people
//   - FPS counter showing processing speed
//   - Press ESC to exit
//
func main() {

	// ---- Step 4a: Open the webcam ----
	// VideoCaptureDevice(0) opens your default camera (index 0).
	// If you have multiple cameras, try 1, 2, etc.
	// A video is just a series of still images ("frames") shown rapidly.
	fmt.Println("Opening webcam...")
	webcam, err := gocv.VideoCaptureDevice(0)

	// In Go, most functions return an error as the last value.
	// Always check it! nil means no error.
	if err != nil {
		fmt.Printf("Error opening webcam: %v\n", err)
		fmt.Println("Make sure a camera is connected and not in use by another app.")
		return
	}

	// defer schedules this function call to run when main() exits.
	// This guarantees the camera is released even if an error occurs later.
	// Think of it like a "finally" block in other languages.
	defer webcam.Close()

	fmt.Println("Webcam opened successfully!")

	// ---- Step 4b: Load the ONNX model ----
	// ReadNetFromONNX loads a pre-trained neural network from an ONNX file.
	// ONNX is a universal model format - you train in Python, run in Go.
	// The model file contains millions of learned parameters (weights) that
	// tell the network how to recognize human body poses.
	fmt.Printf("Loading model from %s...\n", ModelPath)
	net := gocv.ReadNetFromONNX(ModelPath)

	// Check if the model loaded successfully.
	// Empty() returns true if the network has no layers (failed to load).
	if net.Empty() {
		fmt.Println("Error: Could not load ONNX model.")
		fmt.Println("Make sure yolov8s-pose.onnx exists in the current directory.")
		fmt.Println("Export it from Python:")
		fmt.Println("  from ultralytics import YOLO")
		fmt.Println("  YOLO('yolov8s-pose.pt').export(format='onnx')")
		return
	}
	defer net.Close()

	// Try to use CUDA (NVIDIA GPU) for faster inference.
	// If no GPU is available, OpenCV silently falls back to CPU.
	// GPU runs AI models 10-50x faster than CPU.
	net.SetPreferableBackend(gocv.NetBackendCUDA)
	net.SetPreferableTarget(gocv.NetTargetCUDA)

	fmt.Println("Model loaded successfully!")

	// ---- Step 4c: Create a display window ----
	// NewWindow creates an OpenCV window to show the video feed.
	// The string is the window title shown in the title bar.
	window := gocv.NewWindow("Pose Estimation")
	defer window.Close()

	// ---- Step 4d: Prepare reusable Mat objects ----
	// A "Mat" (matrix) is OpenCV's image container - a 2D array of pixel values.
	// We create them once and reuse them each frame to avoid memory allocation
	// overhead in the hot loop. NewMat() creates an empty matrix.
	img := gocv.NewMat()
	defer img.Close()

	blob := gocv.NewMat()
	defer blob.Close()

	fmt.Println("\nPress ESC to quit")
	fmt.Println("Starting pose estimation...\n")

	// Track time between frames to calculate FPS (frames per second).
	// FPS tells you how smoothly the video runs: 30+ = smooth, <15 = choppy.
	prevTime := time.Now()

	// Frame counter for periodic status output to the terminal
	frameCount := 0

	// ---- Step 4e: Main processing loop ----
	// This loop runs continuously until the user presses ESC.
	// Each iteration: capture frame -> preprocess -> inference -> draw -> display.
	for {
		// Read one frame from the camera into our img Mat.
		// Read() returns false if the camera stops working.
		if ok := webcam.Read(&img); !ok || img.Empty() {
			fmt.Println("Error: Could not read frame from camera")
			break
		}

		// Remember the original frame dimensions. We need these later to
		// scale the model's output coordinates back to the original size.
		frameWidth := float32(img.Cols())  // Image width in pixels
		frameHeight := float32(img.Rows()) // Image height in pixels

		// ---- Preprocess: convert image to neural network input ----
		// BlobFromImage converts an image into a "blob" - the format neural
		// networks expect. This involves several transformations:
		//
		// Parameters:
		//   img                          - source image
		//   1.0/255.0                    - scale factor (normalize pixels from 0-255 to 0.0-1.0)
		//   image.Pt(640, 640)           - resize to 640x640 (model's expected input size)
		//   gocv.NewScalar(0, 0, 0, 0)   - mean subtraction (none here)
		//   true                         - swap Red and Blue channels (BGR -> RGB)
		//   false                        - don't crop the image
		//
		// The neural network was trained on 640x640 RGB images with pixel values
		// normalized to 0.0-1.0, so we must match that exact format.
		blob = gocv.BlobFromImage(img, 1.0/255.0, image.Pt(ModelInputSize, ModelInputSize),
			gocv.NewScalar(0, 0, 0, 0), true, false)

		// ---- Run inference ----
		// SetInput feeds our preprocessed blob into the network's input layer.
		// The empty string "" means use the default input layer name.
		net.SetInput(blob, "")

		// Forward runs the neural network! This is where the AI "thinks".
		// Data flows forward through all layers of the network, and we get
		// the output tensor containing all detected poses.
		// The empty string "" means get output from the default output layer.
		output := net.Forward("")

		// ---- Parse the output tensor ----
		// The YOLOv8s-pose ONNX model outputs a tensor of shape (1, 56, N) where:
		//   - 1 = batch size (we process one image at a time)
		//   - 56 = data per detection:
		//       4 values for bounding box (center_x, center_y, width, height)
		//       1 value for detection confidence
		//       51 values for keypoints (17 keypoints x 3 values: x, y, confidence)
		//   - N = number of candidate detections (typically 8400 for 640x640 input)
		//
		// We parse this raw tensor into our Person structs.
		people := parseOutputTensor(&output, frameWidth, frameHeight)
		output.Close()

		// ---- Draw skeletons on the frame ----
		// For each detected person, draw their skeleton overlay.
		for _, person := range people {
			drawSkeleton(&img, person)
		}

		// ---- Calculate and display FPS ----
		currentTime := time.Now()
		// Duration between frames. Seconds() converts to float64 seconds.
		elapsed := currentTime.Sub(prevTime).Seconds()
		prevTime = currentTime

		// FPS = 1 / time_per_frame. Guard against division by zero.
		fps := 1.0 / math.Max(elapsed, 0.001)

		// Draw FPS text on the frame.
		// PutText parameters: image, text, position, font, scale, color, thickness.
		gocv.PutText(&img, fmt.Sprintf("FPS: %.1f", fps), image.Pt(10, 30),
			gocv.FontHersheyPlain, 2.0, textColor, 2)

		// Draw person count
		gocv.PutText(&img, fmt.Sprintf("People: %d", len(people)), image.Pt(10, 60),
			gocv.FontHersheyPlain, 2.0, textColor, 2)

		// ---- Show the frame in the window ----
		window.IMShow(img)

		// Print status to terminal every 30 frames (roughly once per second)
		frameCount++
		if frameCount%30 == 0 {
			fmt.Printf("Frame %d | FPS: %.1f | People detected: %d\n",
				frameCount, fps, len(people))
		}

		// ---- Check for ESC key ----
		// WaitKey(1) waits 1 millisecond for a key press.
		// Returns the key code, or -1 if no key was pressed.
		// ESC key = ASCII code 27.
		key := window.WaitKey(1)
		if key == 27 { // 27 = ESC key
			fmt.Println("\nESC pressed. Exiting...")
			break
		}
	}

	// ---- Step 4f: Cleanup ----
	// All our defer statements run automatically here when main() returns:
	//   - webcam.Close()  releases the camera
	//   - net.Close()     frees the neural network
	//   - window.Close()  closes the display window
	//   - img.Close()     frees image memory
	//   - blob.Close()    frees blob memory
	//
	// Go's defer runs in LIFO order (last in, first out), but order doesn't
	// matter here since these resources are independent of each other.
	fmt.Println("Pose estimation stopped. Goodbye!")
}

// ==============================================================================
// Step 5: Parse the neural network output tensor
// ==============================================================================

// parseOutputTensor converts the raw model output into a slice of Person structs.
//
// The ONNX model outputs shape (1, 56, N):
//   - Rows in the tensor = 56 (each row is one attribute across all detections)
//   - Columns = N candidate detections
//
// We need to "transpose" this mentally: for each detection (column), read its
// 56 values from the rows:
//
//	Row 0: center_x        (bounding box center X)
//	Row 1: center_y        (bounding box center Y)
//	Row 2: width           (bounding box width)
//	Row 3: height          (bounding box height)
//	Row 4: confidence      (detection confidence score)
//	Row 5-7: keypoint 0    (x, y, confidence for nose)
//	Row 8-10: keypoint 1   (x, y, confidence for left eye)
//	...and so on for all 17 keypoints
//
// Parameters:
//   - output: the raw tensor Mat from net.Forward()
//   - frameWidth: original camera frame width (for scaling coordinates)
//   - frameHeight: original camera frame height (for scaling coordinates)
//
// Returns: a slice of Person structs for people above the confidence threshold.
func parseOutputTensor(output *gocv.Mat, frameWidth, frameHeight float32) []Person {
	// The output Mat has shape (1, 56, N). We need to reshape it.
	// Reshape(1, 56) gives us a 2D matrix with 56 rows and N columns.
	// The first parameter (1) is the number of channels.
	reshaped := output.Reshape(1, 56)
	defer reshaped.Close()

	// Number of candidate detections = number of columns
	numDetections := reshaped.Cols()

	// Scale factors to convert from model coordinates (0-640) back to
	// the original frame size. The model works on 640x640 internally,
	// but the camera frame may be a different size (e.g., 1920x1080).
	scaleX := frameWidth / float32(ModelInputSize)
	scaleY := frameHeight / float32(ModelInputSize)

	// Collect all people above the confidence threshold.
	// We use a Go slice (dynamic array) - it grows as we append to it.
	var people []Person

	// Loop through every candidate detection
	for i := 0; i < numDetections; i++ {
		// Read the confidence score for this detection (row 4, column i).
		// GetFloatAt(row, col) reads a single float32 value from the Mat.
		confidence := reshaped.GetFloatAt(4, i)

		// Skip low-confidence detections. Most of the N candidates are
		// background or overlapping detections with low scores.
		if confidence < ConfidenceThreshold {
			continue
		}

		// Read bounding box center and size (rows 0-3)
		cx := reshaped.GetFloatAt(0, i) // Center X in model coordinates
		cy := reshaped.GetFloatAt(1, i) // Center Y in model coordinates
		w := reshaped.GetFloatAt(2, i)  // Width in model coordinates
		h := reshaped.GetFloatAt(3, i)  // Height in model coordinates

		// Convert center+size format to corner format (x1, y1, x2, y2)
		// and scale from model coordinates to original frame coordinates.
		// x1,y1 = top-left corner; x2,y2 = bottom-right corner.
		person := Person{
			Confidence: confidence,
			BBox: [4]float32{
				(cx - w/2) * scaleX, // x1 (left edge)
				(cy - h/2) * scaleY, // y1 (top edge)
				(cx + w/2) * scaleX, // x2 (right edge)
				(cy + h/2) * scaleY, // y2 (bottom edge)
			},
		}

		// Read 17 keypoints (starting at row 5, 3 values per keypoint)
		for kp := 0; kp < 17; kp++ {
			// Each keypoint has 3 consecutive rows: x, y, confidence
			// Keypoint 0 (nose) is at rows 5, 6, 7
			// Keypoint 1 (left eye) is at rows 8, 9, 10
			// General formula: row = 5 + (kp * 3) + offset
			baseRow := 5 + kp*3
			person.Keypoints[kp] = Keypoint{
				X:          reshaped.GetFloatAt(baseRow, i) * scaleX,   // Scale X to frame
				Y:          reshaped.GetFloatAt(baseRow+1, i) * scaleY, // Scale Y to frame
				Confidence: reshaped.GetFloatAt(baseRow+2, i),          // Confidence stays 0-1
			}
		}

		people = append(people, person)

		// Limit the number of people we process to avoid slowdown
		if len(people) >= MaxDetections {
			break
		}
	}

	// Apply simple Non-Maximum Suppression (NMS) to remove duplicate detections.
	// The model often outputs multiple overlapping boxes for the same person.
	// NMS keeps only the highest-confidence detection for each person.
	people = nonMaxSuppression(people, 0.5)

	return people
}

// ==============================================================================
// Step 6: Non-Maximum Suppression (NMS)
// ==============================================================================

// nonMaxSuppression removes duplicate detections of the same person.
//
// When the model sees a person, it often outputs 3-5 overlapping bounding
// boxes around them with slightly different sizes. NMS keeps only the best
// one by checking how much boxes overlap (IoU = Intersection over Union).
//
// How it works:
//  1. Sort detections by confidence (highest first)
//  2. Take the highest-confidence detection, add it to results
//  3. Remove all other detections that overlap too much with it (IoU > threshold)
//  4. Repeat until no detections remain
//
// Parameters:
//   - people: all candidate detections
//   - iouThreshold: max allowed overlap (0.5 = 50% overlap triggers removal)
func nonMaxSuppression(people []Person, iouThreshold float32) []Person {
	if len(people) == 0 {
		return people
	}

	// Sort by confidence, highest first.
	// We use a simple insertion sort since we typically have < 20 detections.
	for i := 1; i < len(people); i++ {
		for j := i; j > 0 && people[j].Confidence > people[j-1].Confidence; j-- {
			people[j], people[j-1] = people[j-1], people[j]
		}
	}

	// keep tracks which detections survive NMS.
	// A boolean slice where true = keep this detection, false = suppressed.
	keep := make([]bool, len(people))
	for i := range keep {
		keep[i] = true // Start by keeping all detections
	}

	// Compare each pair of detections
	for i := 0; i < len(people); i++ {
		if !keep[i] {
			continue // Already suppressed, skip
		}
		for j := i + 1; j < len(people); j++ {
			if !keep[j] {
				continue
			}
			// If boxes overlap too much, suppress the lower-confidence one (j)
			if computeIoU(people[i].BBox, people[j].BBox) > iouThreshold {
				keep[j] = false
			}
		}
	}

	// Build the final list of surviving detections
	var result []Person
	for i, p := range people {
		if keep[i] {
			result = append(result, p)
		}
	}
	return result
}

// computeIoU calculates Intersection over Union between two bounding boxes.
//
// IoU measures how much two boxes overlap, from 0.0 (no overlap) to 1.0
// (identical boxes). It's the standard metric for comparing bounding boxes.
//
//	IoU = Area of Overlap / Area of Union
//
// Parameters: two bounding boxes as [x1, y1, x2, y2] (corner format)
func computeIoU(a, b [4]float32) float32 {
	// Find the overlapping rectangle (intersection)
	// The intersection's left edge is the rightmost of the two left edges.
	// The intersection's right edge is the leftmost of the two right edges.
	x1 := float32(math.Max(float64(a[0]), float64(b[0]))) // Left edge of intersection
	y1 := float32(math.Max(float64(a[1]), float64(b[1]))) // Top edge of intersection
	x2 := float32(math.Min(float64(a[2]), float64(b[2]))) // Right edge of intersection
	y2 := float32(math.Min(float64(a[3]), float64(b[3]))) // Bottom edge of intersection

	// If there's no overlap, the intersection edges will be inverted
	// (right < left or bottom < top), so area = 0.
	interWidth := float32(math.Max(0, float64(x2-x1)))
	interHeight := float32(math.Max(0, float64(y2-y1)))
	interArea := interWidth * interHeight

	// Calculate the area of each box: width * height
	areaA := (a[2] - a[0]) * (a[3] - a[1])
	areaB := (b[2] - b[0]) * (b[3] - b[1])

	// Union = total area of both boxes minus the overlap (to avoid counting it twice)
	unionArea := areaA + areaB - interArea

	// Guard against division by zero (shouldn't happen with valid boxes)
	if unionArea <= 0 {
		return 0
	}

	return interArea / unionArea
}

// ==============================================================================
// Step 7: Draw skeleton overlay on the frame
// ==============================================================================

// drawSkeleton draws a person's skeleton overlay on the image.
// It draws bone connections (lines) first, then joint dots on top,
// so the dots are always visible even at joint intersections.
//
// Parameters:
//   - img: the camera frame to draw on (modified in place via pointer)
//   - person: the detected person with 17 keypoints
func drawSkeleton(img *gocv.Mat, person Person) {

	// ---- Draw skeleton lines (bones) first ----
	// Each connection links two body joints. We only draw a line if BOTH
	// endpoints have high enough confidence (the model is sure about both).
	for _, conn := range skeletonConnections {
		// conn[0] and conn[1] are the keypoint indices to connect
		kp1 := person.Keypoints[conn[0]]
		kp2 := person.Keypoints[conn[1]]

		// Skip this bone if either joint has low confidence
		if kp1.Confidence < KeypointConfidenceThreshold ||
			kp2.Confidence < KeypointConfidenceThreshold {
			continue
		}

		// Convert float coordinates to integer pixel positions.
		// image.Pt(x, y) creates an image.Point struct.
		pt1 := image.Pt(int(kp1.X), int(kp1.Y))
		pt2 := image.Pt(int(kp2.X), int(kp2.Y))

		// Draw a line between the two keypoints.
		// Parameters: image, start point, end point, color, thickness.
		gocv.Line(img, pt1, pt2, skeletonColor, 2)
	}

	// ---- Draw keypoint dots on top ----
	// Drawing dots after lines ensures they appear on top of the bones.
	for i := 0; i < 17; i++ {
		kp := person.Keypoints[i]

		// Only draw if the model is confident about this keypoint
		if kp.Confidence < KeypointConfidenceThreshold {
			continue
		}

		// Draw a filled circle at the keypoint position.
		// Parameters: image, center, radius, color, thickness.
		// Thickness of -1 means "filled circle".
		center := image.Pt(int(kp.X), int(kp.Y))
		gocv.Circle(img, center, 5, keypointColor, -1)
	}
}

// ==============================================================================
// Reference: Key Concepts Explained
// ==============================================================================

// Go concepts used in this example:
//
// package main
//   Every Go program starts with a package declaration.
//   "main" package is special - it's the entry point.
//
// import (...)
//   Import packages we need. Standard library packages (fmt, image, math, time)
//   need no installation. "gocv.io/x/gocv" is an external package that wraps
//   OpenCV's C++ library for use in Go.
//
// const vs var:
//   const = compile-time constant, cannot be changed after declaration.
//   var = variable that can be modified. We use var for slices and colors
//   because Go doesn't allow complex types (slices, structs) as const.
//
// [17]Keypoint vs []Keypoint:
//   [17]Keypoint = fixed-size array, always exactly 17 elements (known at compile time).
//   []Keypoint = slice, dynamic size (can grow with append). We use fixed arrays
//   for keypoints (always 17) and slices for detected people (varies per frame).
//
// defer statement:
//   defer someFunction() schedules someFunction to run when the current
//   function returns. This guarantees cleanup even if an error occurs.
//   Multiple defers run in LIFO (last in, first out) order.
//
// Pointer receivers (*gocv.Mat):
//   &img passes a pointer to img, allowing functions to modify the original.
//   In Go, function arguments are copied by default. Using a pointer means
//   "modify the original image" rather than "modify a copy".
//
// range keyword:
//   "for _, conn := range skeletonConnections" iterates over a slice.
//   _ discards the index (we don't need it), conn is the current element.
//   "for i, p := range people" gives both index (i) and value (p).
//
// GoCV and OpenCV:
//   GoCV (gocv.io/x/gocv) provides Go bindings for OpenCV (Open Source Computer
//   Vision Library). OpenCV is the most widely used computer vision library,
//   written in C++. GoCV wraps it so we can use it from Go without writing C++.
//
//   Key GoCV types:
//     gocv.Mat         - Matrix/image container (wrapper for cv::Mat)
//     gocv.VideoCapture - Camera/video reader
//     gocv.Net          - Neural network for inference
//     gocv.Window       - Display window
//
// ONNX (Open Neural Network Exchange):
//   A universal format for AI models. You can train a model in Python using
//   PyTorch or TensorFlow, export it to ONNX, and run it in Go (or C++, C#,
//   Java, JavaScript, etc.). This is how we use a Python-trained model in Go.
//
// BlobFromImage preprocessing:
//   Neural networks don't understand raw images. BlobFromImage converts an
//   image into the exact format the network expects:
//   1. Resize to 640x640 (the size the model was trained on)
//   2. Normalize pixel values from 0-255 to 0.0-1.0 (divide by 255)
//   3. Swap color channels from BGR to RGB (OpenCV uses BGR, model expects RGB)
//   4. Rearrange dimensions to NCHW format (batch, channels, height, width)
//
// Non-Maximum Suppression (NMS):
//   The model outputs many overlapping detections for each person. NMS is a
//   standard post-processing step that removes duplicates by:
//   1. Keeping the highest-confidence detection
//   2. Removing all others that overlap significantly (IoU > threshold)
//   This is necessary because the model outputs ~8400 candidate boxes, most
//   of which are duplicates or background.
//
// IoU (Intersection over Union):
//   A standard metric for measuring bounding box overlap:
//   IoU = (area where boxes overlap) / (total area covered by both boxes)
//   IoU = 0.0 means no overlap, IoU = 1.0 means identical boxes.
//   Typically, IoU > 0.5 means "same object" for NMS purposes.
//
// Try these experiments:
//   1. Change ConfidenceThreshold to 0.3 - notice more (noisier) detections
//   2. Change keypointColor to color.RGBA{R: 0, G: 0, B: 255} for blue dots
//   3. Try a smaller model: change ModelPath to "yolov8n-pose.onnx" (faster)
//   4. Change ModelInputSize to 320 for faster but less accurate detection
//   5. Have two people stand in front of the camera - it tracks multiple people
//   6. Print keypointNames[i] alongside each keypoint to label body parts

// Unused variable to prevent Go compiler from complaining about keypointNames.
// In a real application, you could use these names to label keypoints on screen.
var _ = keypointNames
