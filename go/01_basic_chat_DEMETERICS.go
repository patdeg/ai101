// Example 1: Basic Chat with Demeterics
//
// This example demonstrates making a Demeterics API call using only
// Go's standard library - no external dependencies needed!
//
// Demeterics uses Groq's OpenAI-compatible protocol.
//
// Run with: go run 01_basic_chat_DEMETERICS.go

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

// Request and response structures for Demeterics API (OpenAI-compatible)
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
	apiKey := os.Getenv("DEMETERICS_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: DEMETERICS_API_KEY not set")
		fmt.Println("Run: export DEMETERICS_API_KEY='your-key-here'")
		os.Exit(1)
	}

	// Step 2: Prepare the request
	request := ChatRequest{
		Model: "meta-llama/llama-4-scout-17b-16e-instruct",
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
	req, err := http.NewRequest("POST", "https://demeterics.uc.r.appspot.com/api/groq/v1/chat/completions", bytes.NewBuffer(jsonData))
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
	fmt.Println("Full Demeterics Response:")
	prettyJSON, _ := json.MarshalIndent(chatResp, "", "  ")
	fmt.Println(string(prettyJSON))

	// Extract the answer
	if len(chatResp.Choices) > 0 {
		fmt.Println("\n" + strings.Repeat("=", 50))
		fmt.Println("AI's Answer:")
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

// Demeterics API Notes:
// - Uses Groq's OpenAI-compatible protocol at Demeterics base URL
// - Same request/response format as OpenAI and Groq
// - Model: meta-llama/llama-4-scout-17b-16e-instruct (matches Groq examples)
// - Authorization uses Bearer token format
