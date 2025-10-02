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
	// Model is the AI model identifier to use for the completion
	// Example: "meta-llama/llama-4-scout-17b-16e-instruct"
	Model string `json:"model"`

	// Messages is the conversation history (array of system/user/assistant messages)
	// This example uses: [system message, user message] to control AI behavior
	Messages []Message `json:"messages"`

	// Temperature controls randomness in responses (0.0 to 2.0)
	// Lower values (0.3-0.5) work better with system prompts for consistent formatting
	Temperature float64 `json:"temperature,omitempty"`

	// MaxTokens limits the length of the AI's response
	// System prompts consume tokens, so budget accordingly
	MaxTokens int `json:"max_tokens,omitempty"`
}

type Message struct {
	// Role identifies who sent this message
	// "system" = behavior instructions, "user" = your question, "assistant" = AI response
	Role string `json:"role"`

	// Content is the actual text of the message
	// For system role: instructions on how to behave (tone, format, constraints)
	// For user role: the actual question or prompt
	Content string `json:"content"`
}

// Response structures - define the JSON we receive from the API
type ChatResponse struct {
	// ID is a unique identifier for this API request
	ID string `json:"id"`

	// Object describes the type of response ("chat.completion")
	Object string `json:"object"`

	// Created is the Unix timestamp when the response was generated
	Created int64 `json:"created"`

	// Model is the actual model used
	Model string `json:"model"`

	// Choices contains the AI's response(s)
	Choices []Choice `json:"choices"`

	// Usage provides token statistics
	Usage Usage `json:"usage"`
}

type Choice struct {
	// Index identifies which choice this is (0-based)
	Index int `json:"index"`

	// Message contains the AI's response (follows system prompt instructions)
	Message Message `json:"message"`

	// FinishReason indicates why the response ended
	// "stop" = completed normally, "length" = hit max_tokens
	FinishReason string `json:"finish_reason"`
}

type Usage struct {
	// PromptTokens includes both system and user message tokens
	PromptTokens int `json:"prompt_tokens"`

	// CompletionTokens is the AI's response length
	CompletionTokens int `json:"completion_tokens"`

	// TotalTokens is prompt + completion
	TotalTokens int `json:"total_tokens"`
}

// MAIN FUNCTION OVERVIEW:
// =======================
// What this example demonstrates:
//   - Using system prompts to control AI behavior
//   - Sending multiple messages in a single request
//   - How message roles affect AI responses
//   - Constraining AI output format with instructions
//
// What you'll learn:
//   - The difference between "system" and "user" message roles
//   - How to shape AI personality and response style
//   - How to set response constraints (length, format, tone)
//   - Building multi-message request arrays in Go
//
// Expected output:
//   - Display of the system prompt used
//   - Display of the user question
//   - AI response that follows the system prompt rules
//   - Token usage count
//
func main() {
	// Step 1: Get API key from environment variable
	// Same as in basic_chat - we need authentication
	apiKey := os.Getenv("GROQ_API_KEY")

	// Verify API key is set
	if apiKey == "" {
		fmt.Println("Error: GROQ_API_KEY environment variable not set")
		fmt.Println("Run: export GROQ_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Step 2: Create the request payload with TWO messages
	// The key difference from example 01: we now have TWO messages in our array:
	// 1. System message - tells the AI how to behave
	// 2. User message - the actual question
	//
	// The order matters! System message should come first
	request := ChatRequest{
		Model: "meta-llama/llama-4-scout-17b-16e-instruct",
		Messages: []Message{
			{
				// First message: System prompt defines AI behavior and constraints
				// This is like giving the AI a job description and rules to follow
				Role:    "system",
				Content: "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more.",
			},
			{
				// Second message: User question is what we actually want to know
				// The AI will answer this while following the system rules above
				Role:    "user",
				Content: "Why did the Roman Empire fall?",
			},
		},
		Temperature: 0.5, // Lower than basic_chat (0.7) for more focused, factual responses
		MaxTokens:   150, // Enough for a 2-sentence response
	}

	// Step 3: Convert struct to JSON
	// Marshal converts our Go struct (with 2 messages) into JSON format
	jsonData, err := json.Marshal(request)

	// Check for marshaling errors
	if err != nil {
		fmt.Printf("Error creating JSON: %v\n", err)
		os.Exit(1)
	}

	// Step 4: Create HTTP request
	// Build the POST request to send to the API
	req, err := http.NewRequest(
		"POST",                                            // HTTP method
		"https://api.groq.com/openai/v1/chat/completions", // API endpoint (same for all examples)
		bytes.NewBuffer(jsonData),                         // Request body containing our 2 messages
	)

	// Check if request creation succeeded
	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	// Step 5: Set HTTP headers
	// Headers provide metadata about our request
	req.Header.Set("Content-Type", "application/json") // Tell server we're sending JSON
	req.Header.Set("Authorization", "Bearer "+apiKey)  // Authenticate with our API key

	// Step 6: Send the HTTP request
	// Create HTTP client and execute the request
	client := &http.Client{}
	resp, err := client.Do(req)

	// Check if the request was sent successfully
	if err != nil {
		fmt.Printf("Error sending request: %v\n", err)
		os.Exit(1)
	}

	// Clean up the response body when we're done (defer runs at function exit)
	defer resp.Body.Close()

	// Step 7: Read the response body
	// Read all bytes from the response
	body, err := io.ReadAll(resp.Body)

	// Check if reading succeeded
	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	// Step 8: Parse the JSON response
	// Declare a variable to hold the parsed response
	var response ChatResponse

	// Unmarshal (deserialize) JSON bytes into our Go struct
	err = json.Unmarshal(body, &response)

	// Check if parsing succeeded
	if err != nil {
		fmt.Printf("Error parsing JSON: %v\n", err)
		fmt.Printf("Raw response: %s\n", string(body))
		os.Exit(1)
	}

	// Step 9: Display results
	// Show both the system prompt and user question so we can see
	// how the system prompt influenced the response

	// Display the system prompt we used
	fmt.Println("System Prompt Used:")
	fmt.Println(request.Messages[0].Content) // First message (index 0) is system

	// Display the user question
	fmt.Println("\nUser Question:")
	fmt.Println(request.Messages[1].Content) // Second message (index 1) is user

	// Display the AI's response
	// Notice how it follows the system prompt rules (exactly 2 sentences)
	fmt.Println("\nAI Response (following system rules):")
	fmt.Println(response.Choices[0].Message.Content)

	// Display token usage
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
