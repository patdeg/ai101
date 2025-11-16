package main

// Example 1: Basic Chat
// This sends a simple question to the AI model using Go's standard library.
// No external dependencies needed!

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

// Request structures - define the JSON we send to the API
type ChatRequest struct {
	// Model is the AI model identifier to use for the completion
	// Example: "meta-llama/llama-4-scout-17b-16e-instruct"
	Model string `json:"model"`

	// Messages is the conversation history (array of user/assistant messages)
	// Each message has a role ("user" or "assistant") and content (the text)
	Messages []Message `json:"messages"`

	// Temperature controls randomness in responses (0.0 to 2.0)
	// 0.0 = deterministic/focused, 1.0 = balanced, 2.0 = very creative/random
	// Optional field (omitempty means it won't be sent if zero value)
	Temperature float64 `json:"temperature,omitempty"`

	// MaxTokens limits the length of the AI's response
	// 1 token ≈ 0.75 words. Typical values: 100-2000
	// Optional field
	MaxTokens int `json:"max_tokens,omitempty"`
}

type Message struct {
	// Role identifies who sent this message
	// Valid values: "user" (your input), "assistant" (AI response), "system" (behavior instructions)
	Role string `json:"role"`

	// Content is the actual text of the message
	Content string `json:"content"`
}

// Response structures - define the JSON we receive from the API
type ChatResponse struct {
	// ID is a unique identifier for this API request
	// Example: "chatcmpl-123abc"
	ID string `json:"id"`

	// Object describes the type of response
	// Always "chat.completion" for chat completions
	Object string `json:"object"`

	// Created is the Unix timestamp (seconds since 1970) when the response was generated
	Created int64 `json:"created"`

	// Model is the actual model that was used (may differ from requested if fallback occurred)
	Model string `json:"model"`

	// Choices contains the AI's response(s) - usually just one choice
	Choices []Choice `json:"choices"`

	// Usage provides token consumption statistics for billing/monitoring
	Usage Usage `json:"usage"`
}

type Choice struct {
	// Index identifies which choice this is (0-based)
	// Multiple choices can be requested, but we typically use just one
	Index int `json:"index"`

	// Message contains the AI's actual response
	Message Message `json:"message"`

	// FinishReason indicates why the response ended
	// Values: "stop" (natural end), "length" (hit max_tokens), "content_filter" (safety)
	FinishReason string `json:"finish_reason"`
}

type Usage struct {
	// PromptTokens is the number of tokens in your input (question + context)
	// Used to calculate input costs
	PromptTokens int `json:"prompt_tokens"`

	// CompletionTokens is the number of tokens in the AI's response
	// Used to calculate output costs (usually more expensive than input)
	CompletionTokens int `json:"completion_tokens"`

	// TotalTokens is the sum of prompt + completion tokens
	// Total cost = (PromptTokens × input_price) + (CompletionTokens × output_price)
	TotalTokens int `json:"total_tokens"`
}

// MAIN FUNCTION OVERVIEW:
// =======================
// What this example demonstrates:
//   - Basic HTTP POST request to Groq AI API
//   - JSON serialization and deserialization in Go
//   - Error handling patterns in Go
//   - Working with environment variables
//
// What you'll learn:
//   - How to structure API requests with Go's standard library
//   - How to handle JSON data without external dependencies
//   - How to read API responses and extract information
//   - Go's idiomatic error handling approach
//
// Expected output:
//   - Full JSON response from the API (prettified)
//   - The AI's answer to your question
//   - Token usage statistics (prompt, completion, total)
//
func main() {
	// Step 1: Get API key from environment variable
	// The API key is sensitive information, so we store it as an environment variable
	// rather than hardcoding it in the source code
	apiKey := os.Getenv("DEMETERICS_API_KEY")

	// Check if API key exists before proceeding
	if apiKey == "" {
		fmt.Println("Error: DEMETERICS_API_KEY environment variable not set")
		fmt.Println("Run: export DEMETERICS_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Step 2: Create the request payload
	// We build a ChatRequest struct that will be converted to JSON
	// This struct contains all the information the API needs
	request := ChatRequest{
		Model: "meta-llama/llama-4-scout-17b-16e-instruct", // AI model to use
		Messages: []Message{
			{
				Role:    "user",                               // "user" means this is our question
				Content: "What is the capital of Switzerland?", // The actual question
			},
		},
		Temperature: 0.7, // Controls randomness (0.0 = deterministic, 2.0 = very random)
		MaxTokens:   100, // Maximum length of response
	}

	// Step 3: Convert struct to JSON
	// json.Marshal converts our Go struct into JSON bytes
	// This is necessary because HTTP requests send data as bytes
	jsonData, err := json.Marshal(request)

	// Always check for errors in Go!
	if err != nil {
		fmt.Printf("Error creating JSON: %v\n", err)
		os.Exit(1)
	}

	// Step 4: Create HTTP request
	// http.NewRequest creates a new HTTP request object
	// We need to specify the method (POST), URL, and body (our JSON data)
	req, err := http.NewRequest(
		"POST",                                            // HTTP method
		"https://api.demeterics.com/groq/v1/chat/completions", // API endpoint
		bytes.NewBuffer(jsonData),                         // Request body (our JSON data)
	)

	// Check if request creation succeeded
	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	// Step 5: Set HTTP headers
	// Headers tell the server what kind of data we're sending and how to authenticate
	req.Header.Set("Content-Type", "application/json")    // We're sending JSON
	req.Header.Set("Authorization", "Bearer "+apiKey)     // API key for authentication

	// Step 6: Send the HTTP request
	// Create an HTTP client and use it to send our request
	client := &http.Client{} // &http.Client{} creates a new HTTP client with default settings
	resp, err := client.Do(req)

	// Check if the request was sent successfully
	if err != nil {
		fmt.Printf("Error sending request: %v\n", err)
		os.Exit(1)
	}

	// defer ensures resp.Body.Close() runs when main() exits
	// This is critical to prevent resource leaks
	defer resp.Body.Close()

	// Step 7: Read the response body
	// io.ReadAll reads all bytes from the response body
	body, err := io.ReadAll(resp.Body)

	// Check if reading the response succeeded
	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	// Step 8: Parse the JSON response
	// Create a variable to hold the parsed response
	var response ChatResponse

	// json.Unmarshal converts JSON bytes back into a Go struct
	// &response is a pointer so Unmarshal can modify the variable
	err = json.Unmarshal(body, &response)

	// Check if JSON parsing succeeded
	if err != nil {
		fmt.Printf("Error parsing JSON: %v\n", err)
		fmt.Printf("Raw response: %s\n", string(body))
		os.Exit(1)
	}

	// Step 9: Display results
	// Now that we have the parsed response, we can extract and display the information

	// Display the full response in a pretty format
	fmt.Println("Full Response:")
	prettyJSON, _ := json.MarshalIndent(response, "", "  ") // MarshalIndent adds formatting
	fmt.Println(string(prettyJSON))

	// Display just the AI's answer
	fmt.Println("\nAI Answer:")
	fmt.Println(response.Choices[0].Message.Content) // Access the first (and usually only) choice

	// Display token usage statistics
	// Tokens are units of text (roughly 4 characters per token)
	fmt.Println("\nToken Usage:")
	fmt.Printf("  Prompt: %d\n", response.Usage.PromptTokens)        // Tokens in our question
	fmt.Printf("  Response: %d\n", response.Usage.CompletionTokens)  // Tokens in AI's answer
	fmt.Printf("  Total: %d\n", response.Usage.TotalTokens)          // Sum of both
}

// Go concepts explained:
//
// package main
//   Every Go program starts with a package declaration
//   "main" package is special - it's the entry point
//
// import (...)
//   Import packages (libraries) we need
//   Standard library packages don't need installation
//
// type MyStruct struct {...}
//   Defines a custom data type (like a class in other languages)
//   Fields must start with capital letter to be exported (public)
//
// `json:"field_name"`
//   Struct tags - tell json.Marshal/Unmarshal how to handle fields
//   "omitempty" = don't include if zero value (0, "", nil, etc.)
//
// func main() {...}
//   The entry point of the program
//   Must be in package main
//   No parameters, no return value
//
// := operator
//   Short variable declaration
//   apiKey := "value" is same as: var apiKey string = "value"
//   Type is inferred from the value
//
// Error handling pattern:
//   if err != nil {
//       // handle error
//       return
//   }
//   Go doesn't have exceptions - errors are values
//   Always check errors!
//
// os.Getenv("VAR")
//   Gets environment variable (like $DEMETERICS_API_KEY in bash)
//   Returns empty string if not set
//
// os.Exit(1)
//   Exits program with status code
//   0 = success, non-zero = error
//
// json.Marshal(v)
//   Converts Go value to JSON bytes
//   Returns ([]byte, error)
//
// json.Unmarshal(data, &v)
//   Parses JSON bytes into Go value
//   &v = pointer to variable (so it can be modified)
//
// bytes.NewBuffer(data)
//   Creates an io.Reader from byte slice
//   Required for http.NewRequest body
//
// http.NewRequest(method, url, body)
//   Creates HTTP request
//   method = "GET", "POST", etc.
//   body can be nil for GET requests
//
// req.Header.Set(key, value)
//   Sets HTTP header
//   Use Add() to append multiple values
//
// client.Do(req)
//   Sends HTTP request
//   Returns response and error
//
// defer resp.Body.Close()
//   Schedules function call to run when current function returns
//   Ensures resources are cleaned up
//   Always defer Close() after checking error!
//
// io.ReadAll(reader)
//   Reads all data from io.Reader
//   Returns ([]byte, error)
//
// fmt.Printf("format", args...)
//   Formatted print
//   %v = default format
//   %d = decimal integer
//   %s = string
//   %f = float
//
// Capitalization matters:
//   apiKey = private (can't be used outside this package)
//   ApiKey = public (can be imported by other packages)
//
// Arrays vs Slices:
//   [5]int        = array, fixed size
//   []int         = slice, dynamic size (like arrays in other languages)
//   []Message{...} = slice literal
//
// Make vs literal:
//   make([]int, 5)     = create slice with length 5
//   []int{1, 2, 3}     = create slice from values
//   make(map[string]int) = create empty map
