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

	// Create the request payload
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
	fmt.Println("Full Response:")
	prettyJSON, _ := json.MarshalIndent(response, "", "  ")
	fmt.Println(string(prettyJSON))

	fmt.Println("\nAI Answer:")
	fmt.Println(response.Choices[0].Message.Content)

	fmt.Println("\nToken Usage:")
	fmt.Printf("  Prompt: %d\n", response.Usage.PromptTokens)
	fmt.Printf("  Response: %d\n", response.Usage.CompletionTokens)
	fmt.Printf("  Total: %d\n", response.Usage.TotalTokens)
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
//   Gets environment variable (like $GROQ_API_KEY in bash)
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
