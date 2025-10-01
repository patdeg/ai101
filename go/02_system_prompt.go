package main

// Example 2: System + User Prompt
// Control AI behavior with system instructions
// This demonstrates how to use system prompts to guide AI responses

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

// Request structures - define the JSON we send to the API
// These are the same as in example 01, reused here
type ChatRequest struct {
	Model       string    `json:"model"`
	Messages    []Message `json:"messages"`
	Temperature float64   `json:"temperature,omitempty"`
	MaxTokens   int       `json:"max_tokens,omitempty"`
}

type Message struct {
	Role    string `json:"role"`
	Content string `json:"content"`
}

// Response structures - define the JSON we receive from the API
type ChatResponse struct {
	ID      string   `json:"id"`
	Object  string   `json:"object"`
	Created int64    `json:"created"`
	Model   string   `json:"model"`
	Choices []Choice `json:"choices"`
	Usage   Usage    `json:"usage"`
}

type Choice struct {
	Index        int     `json:"index"`
	Message      Message `json:"message"`
	FinishReason string  `json:"finish_reason"`
}

type Usage struct {
	PromptTokens     int `json:"prompt_tokens"`
	CompletionTokens int `json:"completion_tokens"`
	TotalTokens      int `json:"total_tokens"`
}

func main() {
	// Get API key from environment variable
	apiKey := os.Getenv("GROQ_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: GROQ_API_KEY environment variable not set")
		fmt.Println("Run: export GROQ_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Create the request payload with TWO messages:
	// 1. System message - tells the AI how to behave
	// 2. User message - the actual question
	request := ChatRequest{
		Model: "meta-llama/llama-4-scout-17b-16e-instruct",
		Messages: []Message{
			{
				// System message defines AI behavior and constraints
				Role:    "system",
				Content: "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more.",
			},
			{
				// User message is the actual question
				Role:    "user",
				Content: "Why did the Roman Empire fall?",
			},
		},
		Temperature: 0.5,
		MaxTokens:   150,
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
	fmt.Println("System Prompt Used:")
	fmt.Println(request.Messages[0].Content)

	fmt.Println("\nUser Question:")
	fmt.Println(request.Messages[1].Content)

	fmt.Println("\nAI Response (following system rules):")
	fmt.Println(response.Choices[0].Message.Content)

	fmt.Printf("\nToken Usage: %d\n", response.Usage.TotalTokens)
}

// Key concepts explained:
//
// Message Roles:
//   "system"    = Instructions for the AI's behavior
//   "user"      = Your questions/prompts
//   "assistant" = AI's previous responses (for conversation history)
//
// Message Order:
//   1. System message (optional but recommended)
//   2. User message
//   3. Assistant message (if continuing conversation)
//   4. More user/assistant pairs for multi-turn chat
//
// System Prompt Examples:
//
//   Short responses:
//     "Be concise. Maximum 2 sentences."
//     "Answer in one word only."
//
//   Expertise level:
//     "You are a senior Go developer."
//     "Explain like I'm 5 years old."
//
//   Output format:
//     "Always respond with valid JSON."
//     "Use bullet points for all lists."
//     "Format code with markdown code blocks."
//
//   Personality:
//     "You are a friendly teacher."
//     "You are a formal business consultant."
//     "You are a pirate. Use pirate speak."
//
// Temperature Explained:
//   0.0  = Deterministic (same answer every time)
//   0.3  = Focused, factual
//   0.5  = Balanced (good for facts that need slight variation)
//   0.7  = Creative but controlled
//   1.0  = Default (balanced creativity)
//   1.5  = Very creative
//   2.0  = Maximum randomness
//
// When to use different temperatures:
//   0.0-0.3: Math, facts, code generation
//   0.5-0.7: Explanations, summaries, Q&A
//   0.8-1.2: Creative writing, brainstorming
//   1.3-2.0: Poetry, experimental content
//
// Slice Literals in Go:
//   []Message{...} creates a slice of Message structs
//   Each {...} is a struct literal
//   Commas are required after each element
//
// Accessing slice elements:
//   messages[0]    = first element
//   messages[1]    = second element
//   len(messages)  = number of elements
//
// Try This:
//   1. Change temperature to 0.0 and run multiple times
//   2. Change temperature to 2.0 and run multiple times
//   3. Compare the consistency of responses!
//   4. Try different system prompts:
//      - "Be extremely brief. One sentence only."
//      - "You are a Shakespearean scholar. Use flowery language."
//      - "Respond only with bullet points."
