package main

/*
06_image_safety_check.go - Check if image content is safe using LlamaGuard Vision

This example demonstrates how to:
1. Load and encode an image in base64
2. Send it to LlamaGuard for safety analysis
3. Parse the moderation response

LlamaGuard checks images for:
- Violence and graphic content
- Sexual content
- Hate symbols or offensive imagery
- Self-harm content
- Dangerous or illegal activities

Usage:
    go run 06_image_safety_check.go

Environment:
    GROQ_API_KEY - Your Groq API key
*/

import (
	"bytes"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
)

// ContentItem represents text or image content in a message
type ContentItem struct {
	Type     string    `json:"type"`
	Text     string    `json:"text,omitempty"`
	ImageURL *ImageURL `json:"image_url,omitempty"`
}

// ImageURL contains the image data URL
type ImageURL struct {
	URL string `json:"url"`
}

// Message represents a chat message with multi-modal content
type Message struct {
	Role    string        `json:"role"`
	Content []ContentItem `json:"content"`
}

// ChatRequest represents the API request structure
type ChatRequest struct {
	Model     string    `json:"model"`
	Messages  []Message `json:"messages"`
	MaxTokens int       `json:"max_tokens"`
}

// Choice represents a response choice
type Choice struct {
	Message struct {
		Role    string `json:"role"`
		Content string `json:"content"`
	} `json:"message"`
	Index        int    `json:"index"`
	FinishReason string `json:"finish_reason"`
}

// Usage represents token usage statistics
type Usage struct {
	PromptTokens     int `json:"prompt_tokens"`
	CompletionTokens int `json:"completion_tokens"`
	TotalTokens      int `json:"total_tokens"`
}

// ChatResponse represents the API response
type ChatResponse struct {
	ID      string   `json:"id"`
	Object  string   `json:"object"`
	Created int64    `json:"created"`
	Model   string   `json:"model"`
	Choices []Choice `json:"choices"`
	Usage   Usage    `json:"usage"`
	Error   *struct {
		Message string `json:"message"`
		Type    string `json:"type"`
	} `json:"error,omitempty"`
}

// checkImageSafety checks if an image is safe using LlamaGuard Vision
func checkImageSafety(imagePath string) (*ChatResponse, error) {
	// Step 1: Check API key
	apiKey := os.Getenv("GROQ_API_KEY")
	if apiKey == "" {
		return nil, fmt.Errorf("GROQ_API_KEY environment variable not set")
	}

	// Step 2: Load and encode the image
	fmt.Printf("Loading image: %s\n", imagePath)

	// Read the image file
	imageData, err := os.ReadFile(imagePath)
	if err != nil {
		return nil, fmt.Errorf("failed to read image: %w", err)
	}

	// Encode to base64
	imageBase64 := base64.StdEncoding.EncodeToString(imageData)

	// Display image info
	fmt.Printf("Image size: %d bytes\n", len(imageData))
	fmt.Printf("Base64 size: %d characters\n", len(imageBase64))

	// Step 3: Prepare the API request
	request := ChatRequest{
		Model: "meta-llama/llama-guard-4-12b",
		Messages: []Message{
			{
				Role: "user",
				Content: []ContentItem{
					{
						Type: "text",
						Text: "Check this image for safety",
					},
					{
						Type: "image_url",
						ImageURL: &ImageURL{
							URL: fmt.Sprintf("data:image/jpeg;base64,%s", imageBase64),
						},
					},
				},
			},
		},
		MaxTokens: 100, // Safety responses are typically short
	}

	// Convert request to JSON
	jsonData, err := json.Marshal(request)
	if err != nil {
		return nil, fmt.Errorf("failed to marshal request: %w", err)
	}

	// Step 4: Make the API request
	fmt.Println("\nChecking image safety with LlamaGuard...")

	req, err := http.NewRequest("POST", "https://api.groq.com/openai/v1/chat/completions", bytes.NewBuffer(jsonData))
	if err != nil {
		return nil, fmt.Errorf("failed to create request: %w", err)
	}

	// Set headers
	req.Header.Set("Authorization", "Bearer "+apiKey)
	req.Header.Set("Content-Type", "application/json")

	// Make the request
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return nil, fmt.Errorf("request failed: %w", err)
	}
	defer resp.Body.Close()

	// Read the response
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("failed to read response: %w", err)
	}

	// Step 5: Parse the response
	var result ChatResponse
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, fmt.Errorf("failed to parse response: %w", err)
	}

	// Check for API errors
	if result.Error != nil {
		return nil, fmt.Errorf("API error: %s", result.Error.Message)
	}

	return &result, nil
}

// SafetyResult represents the interpretation of a safety check
type SafetyResult struct {
	IsSafe      bool
	Category    string
	Explanation string
}

// interpretSafetyResult interprets the LlamaGuard safety check result
func interpretSafetyResult(response *ChatResponse) SafetyResult {
	// Check if we have a valid response
	if response == nil || len(response.Choices) == 0 {
		return SafetyResult{
			IsSafe:      false,
			Category:    "error",
			Explanation: "No response from safety check",
		}
	}

	// Get the AI's response
	content := strings.ToLower(response.Choices[0].Message.Content)

	// LlamaGuard typically returns "safe" or describes the unsafe category
	isSafe := strings.HasPrefix(content, "safe")

	// Parse the category if unsafe
	if !isSafe {
		// Common unsafe categories from LlamaGuard
		categories := map[string]string{
			"violence":  "Violence or graphic content",
			"sexual":    "Sexual or suggestive content",
			"hate":      "Hate speech or discriminatory content",
			"self-harm": "Self-harm or suicide content",
			"illegal":   "Illegal or dangerous activities",
			"deception": "Deceptive or misleading content",
			"privacy":   "Privacy violation",
			"children":  "Child safety concern",
		}

		// Find which category was flagged
		for key, description := range categories {
			if strings.Contains(content, key) {
				return SafetyResult{
					IsSafe:      false,
					Category:    key,
					Explanation: description,
				}
			}
		}

		// Generic unsafe
		return SafetyResult{
			IsSafe:      false,
			Category:    "unsafe",
			Explanation: content,
		}
	}

	return SafetyResult{
		IsSafe:      true,
		Category:    "safe",
		Explanation: "Image passed safety checks",
	}
}

// fileExists checks if a file exists
func fileExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

func main() {
	// Example image paths to check
	testImages := []string{
		"test_image.jpg",    // Default test image
		"../test_image.jpg", // Try parent directory
		"image.jpg",         // Alternative name
	}

	// Find first available image
	var imagePath string
	for _, path := range testImages {
		if fileExists(path) {
			imagePath = path
			break
		}
	}

	if imagePath == "" {
		fmt.Println("No test image found. Please provide an image file.")
		fmt.Println("Expected one of:", strings.Join(testImages, ", "))
		return
	}

	// Check image safety
	response, err := checkImageSafety(imagePath)
	if err != nil {
		fmt.Printf("Error checking image safety: %v\n", err)
		return
	}

	// Interpret results
	result := interpretSafetyResult(response)

	// Display results
	fmt.Println("\n" + strings.Repeat("=", 50))
	fmt.Println("SAFETY CHECK RESULTS")
	fmt.Println(strings.Repeat("=", 50))
	fmt.Printf("Image: %s\n", imagePath)

	// Display status with emoji
	statusEmoji := "✅"
	if !result.IsSafe {
		statusEmoji = "⚠️"
	}
	fmt.Printf("Status: %s %s\n", statusEmoji, strings.ToUpper(result.Category))
	fmt.Printf("Category: %s\n", result.Category)
	fmt.Printf("Details: %s\n", result.Explanation)

	// Show raw response
	if len(response.Choices) > 0 {
		fmt.Printf("\nRaw response: %s\n", response.Choices[0].Message.Content)
	}

	// Display usage statistics
	if response.Usage.TotalTokens > 0 {
		fmt.Println("\n" + strings.Repeat("=", 50))
		fmt.Println("USAGE STATISTICS")
		fmt.Println(strings.Repeat("=", 50))
		fmt.Printf("Prompt tokens: %d\n", response.Usage.PromptTokens)
		fmt.Printf("Completion tokens: %d\n", response.Usage.CompletionTokens)
		fmt.Printf("Total tokens: %d\n", response.Usage.TotalTokens)

		// Calculate approximate cost (LlamaGuard pricing)
		// $0.20 per 1M tokens (both input and output)
		cost := float64(response.Usage.TotalTokens) / 1_000_000 * 0.20
		fmt.Printf("Estimated cost: $%.6f\n", cost)
	}

	// Educational notes
	fmt.Println("\n" + strings.Repeat("=", 50))
	fmt.Println("💡 EDUCATIONAL NOTES")
	fmt.Println(strings.Repeat("=", 50))
	fmt.Println("- LlamaGuard can analyze images for multiple safety categories")
	fmt.Println("- Always check images before processing in production apps")
	fmt.Println("- Consider caching results for frequently checked images")
	fmt.Println("- Combine with text safety checks for complete moderation")
	fmt.Println("- Base64 encoding increases size by ~33%")
	fmt.Println("- Go's encoding/base64 package handles the conversion")
}