package main

// Example 3: Vision - Analyzing Images
// The AI can "see" and analyze images using base64 encoding
// This shows how to send images to multimodal AI models

import (
	"bytes"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

// Request structures for vision API
// Content can be either text OR an array for multimodal messages
type VisionRequest struct {
	Model       string          `json:"model"`
	Messages    []VisionMessage `json:"messages"`
	Temperature float64         `json:"temperature,omitempty"`
	MaxTokens   int             `json:"max_tokens,omitempty"`
}

type VisionMessage struct {
	Role    string        `json:"role"`
	Content []ContentItem `json:"content"` // Array of content items (text + images)
}

// ContentItem can be text or image
type ContentItem struct {
	Type     string    `json:"type"`               // "text" or "image_url"
	Text     string    `json:"text,omitempty"`     // For type="text"
	ImageURL *ImageURL `json:"image_url,omitempty"` // For type="image_url"
}

type ImageURL struct {
	URL string `json:"url"` // Data URL with base64-encoded image
}

// Response structures - same as basic chat
type ChatResponse struct {
	ID      string   `json:"id"`
	Object  string   `json:"object"`
	Created int64    `json:"created"`
	Model   string   `json:"model"`
	Choices []Choice `json:"choices"`
	Usage   Usage    `json:"usage"`
}

type Choice struct {
	Index        int            `json:"index"`
	Message      SimpleMessage  `json:"message"`
	FinishReason string         `json:"finish_reason"`
}

type SimpleMessage struct {
	Role    string `json:"role"`
	Content string `json:"content"`
}

type Usage struct {
	PromptTokens     int `json:"prompt_tokens"`
	CompletionTokens int `json:"completion_tokens"`
	TotalTokens      int `json:"total_tokens"`
}

func main() {
	// Check for API key
	apiKey := os.Getenv("GROQ_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: GROQ_API_KEY environment variable not set")
		fmt.Println("Run: export GROQ_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Path to image file
	imagePath := "./test_image.jpg"

	// Read the image file
	// os.ReadFile reads entire file into memory as []byte
	imageData, err := os.ReadFile(imagePath)
	if err != nil {
		fmt.Printf("Error: Image file '%s' not found\n", imagePath)
		fmt.Println("\nTo create a test image, run:")
		fmt.Println("  curl -o test_image.jpg https://picsum.photos/400/300")
		os.Exit(1)
	}

	// Encode image to base64
	// base64.StdEncoding.EncodeToString converts []byte to base64 string
	imageBase64 := base64.StdEncoding.EncodeToString(imageData)

	// Display image info
	fmt.Printf("Image loaded: %s\n", imagePath)
	fmt.Printf("Image size: %.2f KB\n", float64(len(imageData))/1024)
	fmt.Printf("Base64 size: %.2f KB\n", float64(len(imageBase64))/1024)
	fmt.Println("(Base64 is ~33% larger than original)\n")

	// Detect MIME type from file extension
	// strings.ToLower converts to lowercase
	// filepath.Ext gets file extension (e.g., ".jpg")
	ext := strings.ToLower(filepath.Ext(imagePath))
	var mimeType string
	switch ext {
	case ".png":
		mimeType = "image/png"
	case ".jpg", ".jpeg":
		mimeType = "image/jpeg"
	case ".gif":
		mimeType = "image/gif"
	case ".webp":
		mimeType = "image/webp"
	default:
		mimeType = "image/jpeg" // default
	}

	// Build data URL
	// Format: data:image/jpeg;base64,<base64_string>
	imageURL := fmt.Sprintf("data:%s;base64,%s", mimeType, imageBase64)

	// Create the request with multimodal content
	// Content is an array containing both text and image
	request := VisionRequest{
		Model: "meta-llama/llama-4-scout-17b-16e-instruct",
		Messages: []VisionMessage{
			{
				Role: "user",
				Content: []ContentItem{
					{
						Type: "text",
						Text: "What is in this image? Describe it in detail.",
					},
					{
						Type: "image_url",
						ImageURL: &ImageURL{
							URL: imageURL,
						},
					},
				},
			},
		},
		Temperature: 0.3,
		MaxTokens:   500,
	}

	// Convert struct to JSON
	jsonData, err := json.Marshal(request)
	if err != nil {
		fmt.Printf("Error creating JSON: %v\n", err)
		os.Exit(1)
	}

	// Create HTTP request
	req, err := http.NewRequest(
		"POST",
		"https://api.groq.com/openai/v1/chat/completions",
		bytes.NewBuffer(jsonData),
	)
	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	// Set headers
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+apiKey)

	// Send the request
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		fmt.Printf("Error sending request: %v\n", err)
		os.Exit(1)
	}
	defer resp.Body.Close()

	// Read the response
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	// Parse the JSON response
	var response ChatResponse
	err = json.Unmarshal(body, &response)
	if err != nil {
		fmt.Printf("Error parsing JSON: %v\n", err)
		fmt.Printf("Raw response: %s\n", string(body))
		os.Exit(1)
	}

	// Display results
	fmt.Println("AI Vision Analysis:")
	fmt.Println(strings.Repeat("=", 50))
	fmt.Println(response.Choices[0].Message.Content)
	fmt.Println(strings.Repeat("=", 50))

	fmt.Printf("\nToken Usage: %d tokens\n", response.Usage.TotalTokens)
}

// Go file handling concepts:
//
// os.ReadFile(path)
//   Reads entire file into memory
//   Returns ([]byte, error)
//   Convenience function - automatically closes file
//
// Equivalent long form:
//   file, err := os.Open(path)
//   if err != nil { return err }
//   defer file.Close()
//   data, err := io.ReadAll(file)
//
// Reading files in Go:
//   os.ReadFile(path)           // Read entire file (recommended for small files)
//   io.ReadAll(reader)          // Read all data from reader
//   bufio.NewScanner(file)      // Read line by line (for large files)
//
// File paths:
//   "./file.txt"     = relative to current directory
//   "/path/to/file"  = absolute path
//   "~/file.txt"     = NOT supported in Go (use os.UserHomeDir())
//
// filepath package:
//   filepath.Join("dir", "file.txt")  // OS-specific path separator
//   filepath.Ext("image.jpg")         // Returns ".jpg"
//   filepath.Base("/path/to/file")    // Returns "file"
//   filepath.Dir("/path/to/file")     // Returns "/path/to"
//
// Base64 encoding in Go:
//   base64.StdEncoding.EncodeToString([]byte) // []byte → base64 string
//   base64.StdEncoding.DecodeString(string)   // base64 string → []byte
//   base64.StdEncoding.Encode(dst, src)       // Encode to existing buffer
//
// Example:
//   original := []byte("Hello")
//   encoded := base64.StdEncoding.EncodeToString(original)  // "SGVsbG8="
//   decoded, _ := base64.StdEncoding.DecodeString(encoded)  // []byte("Hello")
//
// String operations:
//   strings.ToLower("ABC")      // "abc"
//   strings.ToUpper("abc")      // "ABC"
//   strings.HasPrefix("hi", "h") // true
//   strings.HasSuffix("hi", "i") // true
//   strings.Contains("hello", "ell") // true
//   strings.Replace("hi", "i", "o", 1) // "ho" (replace 1 occurrence)
//   strings.ReplaceAll("hi hi", "i", "o") // "ho ho" (replace all)
//   strings.Repeat("=", 50)     // "=================================================="
//
// Switch statements:
//   switch value {
//   case "a":
//       // code
//   case "b", "c":  // Multiple values
//       // code
//   default:
//       // fallback
//   }
//
// No fallthrough by default (unlike C/Java):
//   case "a":
//       fmt.Println("a")
//       // Doesn't fall through to next case
//   case "b":
//       fmt.Println("b")
//
// Pointers in Go:
//   &variable  = address of variable
//   *pointer   = value at address
//   *Type      = pointer to Type
//
// Why ImageURL is a pointer (*ImageURL):
//   - Can be nil when not used
//   - Saves memory (only store pointer, not full struct)
//   - Required for omitempty to work correctly with structs
//
// Pointer example:
//   x := 42
//   p := &x     // p is pointer to x
//   *p = 100    // Change value through pointer
//   fmt.Println(x)  // Prints: 100
//
// Data URL format:
//   data:[<media type>][;base64],<data>
//   Example: data:image/jpeg;base64,/9j/4AAQSkZJRg...
//
// Image size limits:
//   Base64 embedded: 4 MB max
//   URL (remote): 20 MB max
//   Resolution: 33 megapixels max
//
// Why base64?
//   - JSON can't contain raw binary data
//   - Base64 converts binary → text (ASCII)
//   - Every 3 bytes → 4 characters
//   - Size increase: ~33%
//
// Content array structure:
//   Multiple content items in one message
//   [
//       {"type": "text", "text": "..."},
//       {"type": "image_url", "image_url": {...}}
//   ]
//
// Try different questions:
//   "What colors are dominant in this image?"
//   "Is there any text visible? If so, what does it say?"
//   "Describe this image for someone who can't see"
//   "What's the mood or atmosphere of this image?"
//   "Count how many people are in this image"
//
// Type conversions in Go:
//   float64(123)        // Convert int to float64
//   int(3.14)          // Convert float64 to int (truncates)
//   string([]byte{65}) // Convert bytes to string ("A")
//   []byte("Hi")       // Convert string to bytes
//
// fmt.Sprintf vs fmt.Printf:
//   fmt.Sprintf("format", args...)  // Returns string
//   fmt.Printf("format", args...)   // Prints to stdout
//
// Format verbs:
//   %s    = string
//   %d    = integer
//   %f    = float (default precision)
//   %.2f  = float with 2 decimal places
//   %v    = default format
//   %+v   = struct with field names
//   %#v   = Go-syntax representation
//   %T    = type of value
