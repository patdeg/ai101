// Example 1: Basic Chat with Anthropic
//
// This example demonstrates making an Anthropic API call using only
// Go's standard library - no external dependencies needed!
//
// Run with: go run 01_basic_chat_ANTHROPIC.go

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

// Request and response structures for Anthropic API
type AnthropicMessage struct {
	Role    string `json:"role"`
	Content string `json:"content"`
}

type AnthropicRequest struct {
	Model       string             `json:"model"`
	Messages    []AnthropicMessage `json:"messages"`
	Temperature float64            `json:"temperature,omitempty"`
	MaxTokens   int                `json:"max_tokens"`
}

type ContentBlock struct {
	Type string `json:"type"`
	Text string `json:"text"`
}

type AnthropicUsage struct {
	InputTokens  int `json:"input_tokens"`
	OutputTokens int `json:"output_tokens"`
}

type AnthropicResponse struct {
	ID           string         `json:"id"`
	Type         string         `json:"type"`
	Role         string         `json:"role"`
	Content      []ContentBlock `json:"content"`
	Model        string         `json:"model"`
	StopReason   string         `json:"stop_reason"`
	Usage        AnthropicUsage `json:"usage"`
	ErrorMessage *struct {
		Type    string `json:"type"`
		Message string `json:"message"`
	} `json:"error,omitempty"`
}

func main() {
	// Step 1: Load API key from environment
	apiKey := os.Getenv("ANTHROPIC_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: ANTHROPIC_API_KEY not set")
		fmt.Println("Run: export ANTHROPIC_API_KEY='sk-ant-...'")
		os.Exit(1)
	}

	// Step 2: Prepare the request
	request := AnthropicRequest{
		Model: "claude-haiku-4-5",
		Messages: []AnthropicMessage{
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
	req, err := http.NewRequest("POST", "https://api.anthropic.com/v1/messages", bytes.NewBuffer(jsonData))
	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	// Step 5: Set headers (Anthropic requires specific headers)
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("x-api-key", apiKey)
	req.Header.Set("anthropic-version", "2023-06-01")

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
	var chatResp AnthropicResponse
	if err := json.Unmarshal(body, &chatResp); err != nil {
		fmt.Printf("Error parsing response: %v\n", err)
		fmt.Printf("Raw response: %s\n", body)
		os.Exit(1)
	}

	// Step 9: Display results
	fmt.Println("Full Anthropic Response:")
	prettyJSON, _ := json.MarshalIndent(chatResp, "", "  ")
	fmt.Println(string(prettyJSON))

	// Extract the answer
	if len(chatResp.Content) > 0 {
		fmt.Println("\n" + strings.Repeat("=", 50))
		fmt.Println("Claude's Answer:")
		fmt.Println(chatResp.Content[0].Text)
	}

	// Show token usage
	fmt.Println("\n" + strings.Repeat("=", 50))
	fmt.Println("Token Usage:")
	fmt.Printf("  Input: %d\n", chatResp.Usage.InputTokens)
	fmt.Printf("  Output: %d\n", chatResp.Usage.OutputTokens)
	fmt.Printf("  Total: %d\n", chatResp.Usage.InputTokens+chatResp.Usage.OutputTokens)

	// Check for errors
	if chatResp.ErrorMessage != nil {
		fmt.Printf("\nAPI Error: %s\n", chatResp.ErrorMessage.Message)
	}
}

// Anthropic API Notes:
// - Uses Messages API (not chat completions)
// - Requires 3 headers: Content-Type, x-api-key, anthropic-version
// - Models: claude-haiku-4-5 (fast), claude-sonnet-4-5 (balanced), claude-opus-4-5 (powerful)
// - Response contains content blocks instead of message objects
// - max_tokens is required (not optional like OpenAI)
