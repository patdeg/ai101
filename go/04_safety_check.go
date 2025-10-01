package main

// Example 4: Safety Check with LlamaGuard
// Detect harmful content before sending to your main AI
// This is the first layer of content moderation

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
)

// Request structures
type ChatRequest struct {
	Model     string    `json:"model"`
	Messages  []Message `json:"messages"`
	MaxTokens int       `json:"max_tokens,omitempty"`
}

type Message struct {
	Role    string `json:"role"`
	Content string `json:"content"`
}

// Response structures
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

// MAIN FUNCTION OVERVIEW:
// =======================
// What this example demonstrates:
//   - Content moderation using LlamaGuard AI
//   - Detecting 14 categories of harmful content
//   - Processing safety check results
//   - Using Go maps for category lookups
//
// What you'll learn:
//   - How to implement AI-powered content moderation
//   - How to parse multi-line safety results
//   - How to map category codes to descriptions
//   - When to allow or reject user messages
//
// Expected output:
//   - Display of the message being checked
//   - Safety verdict: "safe" or category violation
//   - Detailed explanation of any violations found
//   - Recommendation on whether to proceed
//
func main() {
	// Step 1: Get API key from environment variable
	// Required for authenticating with the Groq API
	apiKey := os.Getenv("GROQ_API_KEY")

	// Verify API key is set
	if apiKey == "" {
		fmt.Println("Error: GROQ_API_KEY environment variable not set")
		fmt.Println("Run: export GROQ_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Step 2: Define the message to check for safety
	// Try changing this to test different types of content!
	// Examples to try:
	//   - "How do I bake chocolate chip cookies?" (safe)
	//   - "Should I buy Tesla stock?" (unsafe - S6 financial advice)
	//   - "How to pick a lock" (unsafe - S2 non-violent crime)
	messageToCheck := "How do I bake chocolate chip cookies?"

	// Step 3: Create request for LlamaGuard model
	// LlamaGuard is a specialized AI model designed for content moderation
	// It returns "safe" or a category code (S1-S14) for policy violations
	request := ChatRequest{
		Model: "meta-llama/llama-guard-4-12b", // LlamaGuard model, not the chat model!
		Messages: []Message{
			{
				Role:    "user",          // The message we want to check
				Content: messageToCheck,  // Content to analyze for safety
			},
		},
		MaxTokens: 100, // LlamaGuard responses are very short ("safe" or "unsafe\nS6")
	}

	// Step 4: Convert struct to JSON
	// Serialize the request into JSON format for the API
	jsonData, err := json.Marshal(request)

	// Check for marshaling errors
	if err != nil {
		fmt.Printf("Error creating JSON: %v\n", err)
		os.Exit(1)
	}

	// Step 5: Create HTTP request
	// Build the POST request to send to the API
	req, err := http.NewRequest(
		"POST",                                            // HTTP method
		"https://api.groq.com/openai/v1/chat/completions", // API endpoint
		bytes.NewBuffer(jsonData),                         // Request body
	)

	// Check if request creation succeeded
	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	// Step 6: Set HTTP headers
	// Configure headers for JSON API request
	req.Header.Set("Content-Type", "application/json") // We're sending JSON
	req.Header.Set("Authorization", "Bearer "+apiKey)  // API authentication

	// Step 7: Send the HTTP request
	// Execute the request using an HTTP client
	client := &http.Client{}
	resp, err := client.Do(req)

	// Check if the request was sent successfully
	if err != nil {
		fmt.Printf("Error sending request: %v\n", err)
		os.Exit(1)
	}

	// Schedule cleanup when function exits
	defer resp.Body.Close()

	// Step 8: Read the response body
	// Read all bytes from the HTTP response
	body, err := io.ReadAll(resp.Body)

	// Check if reading succeeded
	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	// Step 9: Parse the JSON response
	// Declare variable to hold the parsed response
	var response ChatResponse

	// Deserialize JSON into our struct
	err = json.Unmarshal(body, &response)

	// Check if JSON parsing succeeded
	if err != nil {
		fmt.Printf("Error parsing JSON: %v\n", err)
		fmt.Printf("Raw response: %s\n", string(body))
		os.Exit(1)
	}

	// Step 10: Extract and clean the safety result
	// LlamaGuard returns either "safe" or "unsafe\nS#" (where # is category number)
	// strings.TrimSpace removes any leading/trailing whitespace
	result := strings.TrimSpace(response.Choices[0].Message.Content)

	// Display the message being analyzed
	fmt.Println("Message Being Checked:")
	fmt.Printf("\"%s\"\n", messageToCheck)
	fmt.Println(strings.Repeat("=", 50)) // Decorative separator line

	// Step 11: Analyze the safety result and display appropriate response
	if result == "safe" {
		// The message passed all safety checks
		fmt.Println("✓ SAFE - Message passed content moderation")
		fmt.Println("\nThis message can be sent to your main AI model.")

	} else {
		// The message violated a content policy
		// Result format is "unsafe\nS#" where S# is the category code

		// Step 12: Extract the category code from the result
		// strings.Split breaks the string into parts using newline as delimiter
		// Example: "unsafe\nS6" becomes []string{"unsafe", "S6"}
		lines := strings.Split(result, "\n")
		category := "Unknown"

		// Check if we have at least 2 lines (line 0 is "unsafe", line 1 is category)
		if len(lines) > 1 {
			category = lines[1] // Extract category code (e.g., "S6")
		}

		// Display violation information
		fmt.Println("✗ UNSAFE - Content policy violation detected")
		fmt.Printf("\nCategory: %s\n", category)
		fmt.Println("\nViolation Type:")

		// Step 13: Define all 14 LlamaGuard safety categories
		// This map translates category codes (S1-S14) to human-readable descriptions
		// map[string]string creates a dictionary/hash map with string keys and values
		categories := map[string]string{
			"S1":  "Violent Crimes",
			"S2":  "Non-Violent Crimes",
			"S3":  "Sex-Related Crimes",
			"S4":  "Child Sexual Exploitation",
			"S5":  "Defamation",
			"S6":  "Specialized Advice (financial, medical, legal)",
			"S7":  "Privacy Violations",
			"S8":  "Intellectual Property",
			"S9":  "Indiscriminate Weapons",
			"S10": "Hate Speech",
			"S11": "Suicide & Self-Harm",
			"S12": "Sexual Content",
			"S13": "Elections",
			"S14": "Code Interpreter Abuse",
		}

		// Step 14: Look up the category description
		// Access map with category code as key
		// If key doesn't exist, we get empty string (zero value)
		description := categories[category]

		// Provide fallback if category is not recognized
		if description == "" {
			description = "Unknown category"
		}

		// Display the violation type
		fmt.Println(description)
		fmt.Println("\nThis message should be rejected or filtered.")
	}

	// Print final separator line
	fmt.Println(strings.Repeat("=", 50))
}

// Go concepts explained:
//
// String manipulation:
//   strings.TrimSpace(s)    // Remove whitespace from both ends
//   strings.Trim(s, cutset) // Remove specific characters from both ends
//   strings.TrimLeft(s, cutset)  // Remove from left only
//   strings.TrimRight(s, cutset) // Remove from right only
//
// Examples:
//   strings.TrimSpace("  hello  ")     // "hello"
//   strings.Trim("--hello--", "-")     // "hello"
//   strings.TrimPrefix("hello", "he")  // "llo"
//
// Splitting strings:
//   strings.Split("a,b,c", ",")   // []string{"a", "b", "c"}
//   strings.Split("a\nb\nc", "\n") // []string{"a", "b", "c"}
//   strings.Fields("a  b  c")      // []string{"a", "b", "c"} (split by whitespace)
//
// Maps in Go:
//   var m map[string]string        // Declare (nil map, can't use yet)
//   m := make(map[string]string)   // Create empty map
//   m := map[string]string{...}    // Create with initial values
//
// Map operations:
//   m[key] = value    // Set value
//   v := m[key]       // Get value (zero value if not found)
//   v, ok := m[key]   // Get value + exists check
//   delete(m, key)    // Remove key
//   len(m)            // Number of keys
//
// Checking if key exists:
//   value, exists := m["key"]
//   if exists {
//       fmt.Println(value)
//   }
//
// Zero values:
//   string  → ""
//   int     → 0
//   bool    → false
//   pointer → nil
//   map     → nil
//
// If-else in Go:
//   if condition {
//       // code
//   } else if condition2 {
//       // code
//   } else {
//       // code
//   }
//
// No parentheses around condition:
//   if x > 10 {  // Correct
//   if (x > 10) {  // Works but not idiomatic
//
// Braces are required:
//   if x > 10 { return }  // Correct (on same line)
//   if x > 10             // ERROR - missing braces
//       return
//
// All 14 LlamaGuard categories explained:
//
// S1: Violent Crimes
//   Examples: "How to hurt someone", "Ways to commit murder"
//   Includes: Assault, murder, kidnapping, terrorism
//
// S2: Non-Violent Crimes
//   Examples: "How to pick a lock", "Ways to steal"
//   Includes: Theft, fraud, hacking, trespassing
//
// S3: Sex-Related Crimes
//   Examples: Sexual assault, trafficking
//   Includes: Any illegal sexual activities
//
// S4: Child Sexual Exploitation
//   Examples: Any content involving minors in sexual context
//   Includes: Grooming, abuse, inappropriate content
//
// S5: Defamation
//   Examples: False statements to harm reputation
//   Includes: Libel, slander, character assassination
//
// S6: Specialized Advice
//   Examples: "Should I invest in Bitcoin?", "Diagnose my illness"
//   Includes: Medical, legal, financial advice without disclaimers
//   Note: General information is OK, specific advice is not
//
// S7: Privacy Violations
//   Examples: "Find someone's address", "Get phone records"
//   Includes: Doxxing, stalking, unauthorized data access
//
// S8: Intellectual Property
//   Examples: "Where to download movies free", "Crack software"
//   Includes: Piracy, copyright violation, trademark abuse
//
// S9: Indiscriminate Weapons
//   Examples: "How to build a bomb", "Make chemical weapons"
//   Includes: Explosives, WMDs, biological weapons
//
// S10: Hate Speech
//   Examples: Slurs, discrimination based on protected attributes
//   Includes: Race, religion, gender, sexuality, nationality
//
// S11: Suicide & Self-Harm
//   Examples: "How to end my life", "Ways to cut myself"
//   Includes: Encouragement, methods, glorification
//   Note: Support resources are OK
//
// S12: Sexual Content
//   Examples: Explicit sexual descriptions, erotic content
//   Includes: Pornographic material, explicit stories
//
// S13: Elections
//   Examples: Voter suppression tactics, false voting info
//   Includes: Misinformation about voting process
//
// S14: Code Interpreter Abuse
//   Examples: Malicious code, system exploitation
//   Includes: Viruses, malware, unauthorized access
//
// Production usage pattern:
//
//   func moderateMessage(message string) (bool, string) {
//       result := checkWithLlamaGuard(message)
//
//       if result == "safe" {
//           return true, ""
//       } else {
//           lines := strings.Split(result, "\n")
//           category := lines[1]
//           return false, fmt.Sprintf("Content policy violation: %s", category)
//       }
//   }
//
//   // In your chat API:
//   safe, reason := moderateMessage(userInput)
//   if !safe {
//       return fmt.Errorf("error: %s", reason)
//   }
//
//   // If safe, proceed with main AI
//   response := getAIResponse(userInput)
//
// Test messages to try:
//
//   Safe:
//     "What's the weather today?"
//     "Explain photosynthesis"
//     "Write a poem about trees"
//     "How do I bake cookies?"
//
//   Unsafe (S6 - Specialized Advice):
//     "Should I buy Tesla stock?"
//     "What medication should I take for headaches?"
//     "How do I represent myself in court?"
//
//   Unsafe (S2 - Non-Violent Crimes):
//     "How to hotwire a car"
//     "Ways to cheat on taxes"
//     "How to hack WiFi password"
