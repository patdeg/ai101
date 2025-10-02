package main

// Example 7: Whisper - Audio Transcription with Whisper Large-v3-Turbo
// Transcribe audio files using fast, cost-effective speech recognition

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"mime/multipart"
	"net/http"
	"os"
	"path/filepath"
	"time"
)

// Response structure for Whisper API
type TranscriptionResponse struct {
	// Text is the complete transcription of the audio file
	// Includes punctuation, capitalization, and formatting
	// Supports 99+ languages with automatic detection
	Text string `json:"text"`

	// Duration is the audio length in seconds
	// Used to calculate cost: (Duration / 3600) × $0.04 per hour
	// Example: 60 seconds = 0.0167 hours = $0.00067
	Duration float64 `json:"duration"`
}

// MAIN FUNCTION OVERVIEW:
// =======================
// What this example demonstrates:
//   - How to transcribe audio files using Whisper large-v3-turbo
//   - Uploading multipart form data to the API
//   - Processing speech-to-text with fast, cost-effective model
//   - Working with file uploads in Go
//
// What you'll learn:
//   - How to use the Whisper API for audio transcription
//   - Working with multipart/form-data in Go
//   - Handling audio file uploads with multipart.Writer
//   - Processing binary file data
//   - Calculating transcription costs
//
// Expected output:
//   - Complete transcription of the Gettysburg Address
//   - JSON response with transcription text
//   - Duration and cost information
//
func main() {
	// Step 1: Get API key from environment variable
	apiKey := os.Getenv("GROQ_API_KEY")

	// Verify API key exists
	if apiKey == "" {
		fmt.Println("Error: GROQ_API_KEY environment variable not set")
		fmt.Println("Run: export GROQ_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Step 2: Prepare the audio file path
	audioFilePath := "../gettysburg.mp3"

	// Check if file exists
	if _, err := os.Stat(audioFilePath); os.IsNotExist(err) {
		fmt.Printf("Error: Audio file not found at %s\n", audioFilePath)
		os.Exit(1)
	}

	// Open the audio file
	file, err := os.Open(audioFilePath)
	if err != nil {
		fmt.Printf("Error opening audio file: %v\n", err)
		os.Exit(1)
	}
	defer file.Close()

	// Get file info for size
	fileInfo, err := file.Stat()
	if err != nil {
		fmt.Printf("Error getting file info: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("============================================================")
	fmt.Println("Transcribing Audio with Whisper")
	fmt.Println("============================================================")
	fmt.Println("Model: whisper-large-v3-turbo")
	fmt.Printf("File: %s\n", audioFilePath)
	fmt.Printf("File size: %d bytes\n", fileInfo.Size())
	fmt.Println()

	// Step 3: Create multipart form data
	// Create a buffer to write the multipart data
	var requestBody bytes.Buffer
	writer := multipart.NewWriter(&requestBody)

	// Add the audio file to the form
	part, err := writer.CreateFormFile("file", filepath.Base(audioFilePath))
	if err != nil {
		fmt.Printf("Error creating form file: %v\n", err)
		os.Exit(1)
	}

	// Copy the file content to the form part
	_, err = io.Copy(part, file)
	if err != nil {
		fmt.Printf("Error copying file content: %v\n", err)
		os.Exit(1)
	}

	// Add the model parameter
	err = writer.WriteField("model", "whisper-large-v3-turbo")
	if err != nil {
		fmt.Printf("Error writing model field: %v\n", err)
		os.Exit(1)
	}

	// Add response format for duration info
	err = writer.WriteField("response_format", "verbose_json")
	if err != nil {
		fmt.Printf("Error writing response_format field: %v\n", err)
		os.Exit(1)
	}

	// Close the multipart writer to finalize the body
	err = writer.Close()
	if err != nil {
		fmt.Printf("Error closing multipart writer: %v\n", err)
		os.Exit(1)
	}

	// Step 4: Create HTTP request
	req, err := http.NewRequest(
		"POST",
		"https://api.groq.com/openai/v1/audio/transcriptions",
		&requestBody,
	)

	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	// Step 5: Set HTTP headers
	req.Header.Set("Authorization", "Bearer "+apiKey)
	req.Header.Set("Content-Type", writer.FormDataContentType())

	// Step 6: Send the HTTP request with timing
	startTime := time.Now()

	client := &http.Client{}
	resp, err := client.Do(req)

	if err != nil {
		fmt.Printf("Error sending request: %v\n", err)
		os.Exit(1)
	}

	defer resp.Body.Close()

	latency := time.Since(startTime).Milliseconds()

	// Step 7: Read the response body
	body, err := io.ReadAll(resp.Body)

	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	// Step 8: Parse the JSON response
	var response TranscriptionResponse

	err = json.Unmarshal(body, &response)

	if err != nil {
		fmt.Printf("Error parsing JSON: %v\n", err)
		fmt.Printf("Raw response: %s\n", string(body))
		os.Exit(1)
	}

	// Step 9: Display the transcription result
	fmt.Println("Transcription Result:")
	fmt.Println("============================================================")
	fmt.Println(response.Text)
	fmt.Println()
	fmt.Println("============================================================")
	fmt.Println()

	// Calculate cost based on audio duration
	cost := (response.Duration / 3600) * 0.04 // $0.04 per hour

	fmt.Println("Performance Metrics:")
	fmt.Println("============================================================")
	fmt.Printf("API Latency:    %dms\n", latency)
	fmt.Printf("Audio Duration: %.2fs\n", response.Duration)
	fmt.Printf("Cost:           $%.6f\n", cost)
	fmt.Println("============================================================")
	fmt.Println()
	fmt.Println("Full API Response:")
	jsonBytes, _ := json.MarshalIndent(response, "", "  ")
	fmt.Println(string(jsonBytes))
}

// Model: whisper-large-v3-turbo
//   - Fast, cost-effective speech recognition from OpenAI
//   - Supports 99+ languages with high accuracy
//   - Automatic language detection
//   - Punctuation and formatting included
//   - Handles various audio formats (mp3, wav, m4a, etc.)
//   - Pricing: $0.04 per hour of audio
//
// Supported audio formats:
//   - MP3, MP4, MPEG, MPGA, M4A, WAV, WEBM
//   - Maximum file size: 25 MB
//   - Recommended: 16kHz or higher sample rate
//
// Pricing examples (at $0.04/hour):
//   - 1 minute:  $0.000667 (~$0.0007)
//   - 5 minutes: $0.003333 (~$0.003)
//   - 30 minutes: $0.02
//   - 1 hour:     $0.04
//   - 10 hours:   $0.40
//
// API Response format:
//   {
//     "text": "The transcribed text will appear here..."
//   }
//
// Use cases:
//   - Meeting transcriptions
//   - Voice notes to text
//   - Podcast transcriptions
//   - Accessibility (captions, subtitles)
//   - Voice interfaces
//   - Call center analytics
//   - Interview transcriptions
//   - Lecture notes
//
// Production code example:
//
//   func transcribeAudio(audioPath string, apiKey string) (string, error) {
//       // Open the audio file
//       file, err := os.Open(audioPath)
//       if err != nil {
//           return "", err
//       }
//       defer file.Close()
//
//       // Create multipart form data
//       var body bytes.Buffer
//       writer := multipart.NewWriter(&body)
//
//       // Add file
//       part, err := writer.CreateFormFile("file", filepath.Base(audioPath))
//       if err != nil {
//           return "", err
//       }
//       io.Copy(part, file)
//
//       // Add model
//       writer.WriteField("model", "whisper-large-v3")
//       writer.Close()
//
//       // Create request
//       req, err := http.NewRequest(
//           "POST",
//           "https://api.groq.com/openai/v1/audio/transcriptions",
//           &body,
//       )
//       if err != nil {
//           return "", err
//       }
//
//       req.Header.Set("Authorization", "Bearer "+apiKey)
//       req.Header.Set("Content-Type", writer.FormDataContentType())
//
//       // Send request
//       client := &http.Client{}
//       resp, err := client.Do(req)
//       if err != nil {
//           return "", err
//       }
//       defer resp.Body.Close()
//
//       // Parse response
//       respBody, err := io.ReadAll(resp.Body)
//       if err != nil {
//           return "", err
//       }
//
//       var result TranscriptionResponse
//       err = json.Unmarshal(respBody, &result)
//       if err != nil {
//           return "", err
//       }
//
//       return result.Text, nil
//   }
//
//   // Usage
//   text, err := transcribeAudio("../gettysburg.mp3", apiKey)
//   if err != nil {
//       log.Fatal(err)
//   }
//   fmt.Println(text)
//
// Advanced options (add to form fields):
//
//   // Specify language (optional - auto-detected by default)
//   writer.WriteField("language", "en")  // ISO-639-1 code (en, es, fr, de, etc.)
//
//   // Response format (optional - json is default)
//   writer.WriteField("response_format", "json")  // Options: json, text, srt, vtt
//
//   // Temperature for randomness (optional - 0.0 to 1.0)
//   writer.WriteField("temperature", "0.0")  // 0.0 = deterministic, 1.0 = creative
//
//   // Timestamp granularities (optional)
//   writer.WriteField("timestamp_granularities[]", "word")  // Options: word, segment
//
// Error handling example:
//
//   func transcribeAudio(audioPath string, apiKey string) (string, error) {
//       // Check if file exists
//       if _, err := os.Stat(audioPath); os.IsNotExist(err) {
//           return "", fmt.Errorf("file not found: %s", audioPath)
//       }
//
//       // Check file size (25MB limit)
//       fileInfo, err := os.Stat(audioPath)
//       if err != nil {
//           return "", err
//       }
//       if fileInfo.Size() > 25*1024*1024 {
//           return "", fmt.Errorf("file size exceeds 25MB limit")
//       }
//
//       // Continue with transcription...
//   }
//
// Saving transcription to file:
//
//   import "os"
//
//   // Save as plain text
//   err = os.WriteFile("transcription.txt", []byte(response.Text), 0644)
//   if err != nil {
//       fmt.Printf("Error saving text: %v\n", err)
//   }
//
//   // Save as JSON
//   jsonData, _ := json.MarshalIndent(response, "", "  ")
//   err = os.WriteFile("transcription.json", jsonData, 0644)
//   if err != nil {
//       fmt.Printf("Error saving JSON: %v\n", err)
//   }
//
// Processing long audio files:
//
//   // For files > 25MB, you'll need to split them
//   // This example uses ffmpeg via exec.Command
//
//   import "os/exec"
//
//   func splitAudio(inputFile string, chunkDuration int) error {
//       // Split audio into chunks using ffmpeg
//       cmd := exec.Command(
//           "ffmpeg",
//           "-i", inputFile,
//           "-f", "segment",
//           "-segment_time", fmt.Sprintf("%d", chunkDuration),
//           "-c", "copy",
//           "chunk_%03d.mp3",
//       )
//       return cmd.Run()
//   }
//
//   func transcribeLongAudio(audioPath string, apiKey string) (string, error) {
//       // Split into 10-minute chunks
//       err := splitAudio(audioPath, 600)
//       if err != nil {
//           return "", err
//       }
//
//       // Get list of chunks
//       chunks, err := filepath.Glob("chunk_*.mp3")
//       if err != nil {
//           return "", err
//       }
//
//       // Transcribe each chunk
//       var transcriptions []string
//       for _, chunk := range chunks {
//           text, err := transcribeAudio(chunk, apiKey)
//           if err != nil {
//               return "", err
//           }
//           transcriptions.append(text)
//           os.Remove(chunk)  // Clean up
//       }
//
//       return strings.Join(transcriptions, " "), nil
//   }
//
// Supported languages (99+):
//   English, Spanish, French, German, Italian, Portuguese, Dutch,
//   Russian, Chinese, Japanese, Korean, Arabic, Turkish, Polish,
//   Ukrainian, Vietnamese, Thai, Indonesian, Hindi, and many more
//
// Language codes (ISO-639-1):
//   en = English
//   es = Spanish
//   fr = French
//   de = German
//   it = Italian
//   pt = Portuguese
//   nl = Dutch
//   ru = Russian
//   zh = Chinese
//   ja = Japanese
//   ko = Korean
//   ar = Arabic
//   tr = Turkish
//   pl = Polish
//   uk = Ukrainian
//   vi = Vietnamese
//   th = Thai
//   id = Indonesian
//   hi = Hindi
//
// Response formats:
//
//   JSON (default):
//     {"text": "Transcription here"}
//
//   Text (plain text):
//     Transcription here
//
//   SRT (SubRip subtitles):
//     1
//     00:00:00,000 --> 00:00:05,000
//     Transcription here
//
//   VTT (WebVTT subtitles):
//     WEBVTT
//
//     00:00:00.000 --> 00:00:05.000
//     Transcription here
//
// Performance tips:
//   - Use MP3 format for smaller file sizes
//   - 16kHz sample rate is usually sufficient
//   - Mono audio reduces file size by ~50%
//   - Remove silence to reduce processing time
//
// Cost optimization:
//   - Whisper pricing is based on audio duration
//   - Remove silence before transcription
//   - Use appropriate quality (don't over-sample)
//   - Cache transcriptions to avoid re-processing
//
// Go concepts explained:
//
// Multipart form data:
//   import "mime/multipart"
//
//   var body bytes.Buffer
//   writer := multipart.NewWriter(&body)
//
//   // Add a file
//   part, err := writer.CreateFormFile("file", "filename.mp3")
//   io.Copy(part, file)
//
//   // Add a text field
//   writer.WriteField("model", "whisper-large-v3")
//
//   // Close to finalize
//   writer.Close()
//
//   // Get content type
//   contentType := writer.FormDataContentType()
//   // Returns: "multipart/form-data; boundary=..."
//
// File operations:
//   // Check if file exists
//   if _, err := os.Stat(path); os.IsNotExist(err) {
//       // File doesn't exist
//   }
//
//   // Get file info
//   info, err := os.Stat(path)
//   info.Size()      // File size in bytes
//   info.Name()      // File name
//   info.ModTime()   // Last modification time
//   info.IsDir()     // Is directory?
//
//   // Open file
//   file, err := os.Open(path)
//   defer file.Close()
//
//   // Read entire file
//   data, err := os.ReadFile(path)
//
//   // Write entire file
//   err = os.WriteFile(path, data, 0644)
//
// Path operations:
//   import "path/filepath"
//
//   filepath.Base("/path/to/file.mp3")   // "file.mp3"
//   filepath.Dir("/path/to/file.mp3")    // "/path/to"
//   filepath.Ext("/path/to/file.mp3")    // ".mp3"
//   filepath.Join("path", "to", "file")  // "path/to/file"
//   filepath.Abs("./file.mp3")           // "/current/dir/file.mp3"
//   filepath.Glob("*.mp3")               // List of matching files
//
// io.Copy:
//   // Copy from reader to writer
//   n, err := io.Copy(dst, src)
//   // n = number of bytes copied
//
//   // Copy file to HTTP request
//   file, _ := os.Open("file.txt")
//   part, _ := writer.CreateFormFile("file", "file.txt")
//   io.Copy(part, file)
//
// Bytes buffer:
//   import "bytes"
//
//   var buf bytes.Buffer
//   buf.Write([]byte("hello"))
//   buf.WriteString("world")
//   buf.Bytes()   // []byte
//   buf.String()  // string
//   buf.Len()     // length
//   buf.Reset()   // clear buffer
//
// HTTP Content-Type for multipart:
//   writer := multipart.NewWriter(&body)
//   // ... add fields ...
//   writer.Close()
//
//   req.Header.Set("Content-Type", writer.FormDataContentType())
//   // Sets: "multipart/form-data; boundary=randomstring"
//
// Defer patterns:
//   file, err := os.Open(path)
//   if err != nil {
//       return err
//   }
//   defer file.Close()  // Executes when function returns
//
//   // Multiple defers execute in reverse order
//   defer fmt.Println("1")
//   defer fmt.Println("2")
//   defer fmt.Println("3")
//   // Output: 3, 2, 1
//
// Error wrapping (Go 1.13+):
//   import "fmt"
//
//   err := doSomething()
//   if err != nil {
//       return fmt.Errorf("failed to do something: %w", err)
//   }
//
//   // Check if error is specific type
//   if errors.Is(err, os.ErrNotExist) {
//       // File doesn't exist
//   }
//
// File size constants:
//   const (
//       KB = 1024
//       MB = 1024 * KB
//       GB = 1024 * MB
//   )
//
//   maxSize := 25 * MB  // 25 MB
//
// Struct tags for JSON:
//   type Response struct {
//       Text     string `json:"text"`              // Maps to "text"
//       Language string `json:"language,omitempty"` // Omit if empty
//       Duration int    `json:"-"`                 // Never serialize
//   }

