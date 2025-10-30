// Example 1: Basic Chat with OpenAI
//
// This example demonstrates making an OpenAI API call using only
// Go's standard library - no external dependencies needed!
//
// Run with: go run 01_basic_chat_OPENAI.go

package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
)

// Request and response structures for OpenAI API
type Message struct {
	Role    string `json:"role"`
	Content string `json:"content"`
}

type ChatRequest struct {
	Model       string    `json:"model"`
	Messages    []Message `json:"messages"`
	Temperature float64   `json:"temperature,omitempty"`
	MaxTokens   int       `json:"max_tokens,omitempty"`
}

type Choice struct {
	Message      Message `json:"message"`
	FinishReason string  `json:"finish_reason"`
}

type Usage struct {
	PromptTokens     int `json:"prompt_tokens"`
	CompletionTokens int `json:"completion_tokens"`
	TotalTokens      int `json:"total_tokens"`
}

type ChatResponse struct {
	ID      string   `json:"id"`
	Choices []Choice `json:"choices"`
	Usage   Usage    `json:"usage"`
	Error   *struct {
		Message string `json:"message"`
		Type    string `json:"type"`
	} `json:"error,omitempty"`
}

func main() {
	// Step 1: Load API key from environment
	apiKey := os.Getenv("OPENAI_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: OPENAI_API_KEY not set")
		fmt.Println("Run: export OPENAI_API_KEY='sk-...'")
		os.Exit(1)
	}

	// Step 2: Prepare the request
	request := ChatRequest{
		Model: "gpt-5-nano",
		Messages: []Message{
			{
				Role:    "user",
				Content: "What is the capital of Switzerland?",
			},
		},
		Temperature: 0.7,
		MaxTokens:   100,
	}

	// Step 3: Marshal request to JSON
	jsonData, err := json.Marshal(request)
	if err != nil {
		fmt.Printf("Error marshaling request: %v\n", err)
		os.Exit(1)
	}

	// Step 4: Create HTTP request
	req, err := http.NewRequest("POST", "https://api.openai.com/v1/chat/completions", bytes.NewBuffer(jsonData))
	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	// Step 5: Set headers
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+apiKey)

	// Step 6: Send the request
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		fmt.Printf("Error sending request: %v\n", err)
		os.Exit(1)
	}
	defer resp.Body.Close()

	// Step 7: Read response
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	// Step 8: Parse response
	var chatResp ChatResponse
	if err := json.Unmarshal(body, &chatResp); err != nil {
		fmt.Printf("Error parsing response: %v\n", err)
		fmt.Printf("Raw response: %s\n", body)
		os.Exit(1)
	}

	// Step 9: Display results
	fmt.Println("Full OpenAI Response:")
	prettyJSON, _ := json.MarshalIndent(chatResp, "", "  ")
	fmt.Println(string(prettyJSON))

	// Extract the answer
	if len(chatResp.Choices) > 0 {
		fmt.Println("\n" + strings.Repeat("=", 50))
		fmt.Println("GPT's Answer:")
		fmt.Println(chatResp.Choices[0].Message.Content)
	}

	// Show token usage
	fmt.Println("\n" + strings.Repeat("=", 50))
	fmt.Println("Token Usage:")
	fmt.Printf("  Prompt: %d\n", chatResp.Usage.PromptTokens)
	fmt.Printf("  Completion: %d\n", chatResp.Usage.CompletionTokens)
	fmt.Printf("  Total: %d\n", chatResp.Usage.TotalTokens)

	// Check for errors
	if chatResp.Error != nil {
		fmt.Printf("\nAPI Error: %s\n", chatResp.Error.Message)
	}
}

// OpenAI API Notes:
// - Uses standard OpenAI chat completions format
// - Models: gpt-5-nano (fast), gpt-4o (powerful)
// - Supports function calling and vision capabilities
// - Rate limits apply based on tier