package main

// Example 4: Safety Check with LlamaGuard - DUAL CHECKS
// Detect harmful content before sending to your main AI
// This is the first layer of content moderation
// Demonstrates TWO checks: SAFE and UNSAFE messages

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
	// Model specifies which AI to use
	// For safety: "meta-llama/llama-guard-4-12b" (specialized content moderator)
	Model string `json:"model"`

	// Messages contains the content to check for safety
	// Role is typically "user" for user-generated content to moderate
	Messages []Message `json:"messages"`

	// MaxTokens limits response length
	// Safety responses are short: "safe" or "unsafe\nS<number>"
	MaxTokens int `json:"max_tokens,omitempty"`
}

type Message struct {
	// Role identifies the content source
	// "user" = check user input before processing
	// "assistant" = check AI output before showing to user
	Role string `json:"role"`

	// Content is the text to check for safety violations
	Content string `json:"content"`
}

// Response structures
type ChatResponse struct {
	// ID uniquely identifies this safety check request
	ID string `json:"id"`

	// Object type is "chat.completion" even for safety checks
	Object string `json:"object"`

	// Created timestamp
	Created int64 `json:"created"`

	// Model is the safety model used (llama-guard-4-12b)
	Model string `json:"model"`

	// Choices contains the safety verdict
	Choices []Choice `json:"choices"`

	// Usage shows token consumption (safety checks are cheap!)
	Usage Usage `json:"usage"`
}

type Choice struct {
	// Index of this choice (always 0 for safety checks)
	Index int `json:"index"`

	// Message contains the safety verdict in content field
	// Content format: "safe" OR "unsafe\nS<number>" (e.g., "unsafe\nS9")
	Message Message `json:"message"`

	// FinishReason is typically "stop" for completed safety checks
	FinishReason string `json:"finish_reason"`
}

type Usage struct {
	// PromptTokens is the content being checked
	PromptTokens int `json:"prompt_tokens"`

	// CompletionTokens is very small (just "safe" or "unsafe\nSX")
	CompletionTokens int `json:"completion_tokens"`

	// TotalTokens for safety checks is usually < 100 tokens (very cheap!)
	TotalTokens int `json:"total_tokens"`
}

// MAIN FUNCTION OVERVIEW:
// =======================
// What this example demonstrates:
//   - Running TWO safety checks to compare safe vs unsafe content
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
//   - TEST 1: Safe message → "safe" verdict
//   - TEST 2: Unsafe message → "unsafe\nS9" (nuclear bomb example)
//   - Both raw API responses for transparency
//
func main() {
	// Step 1: Get API key from environment variable
	// Required for authenticating with the Demeterics Groq proxy
	apiKey := os.Getenv("DEMETERICS_API_KEY")

	// Verify API key is set
	if apiKey == "" {
		fmt.Println("Error: DEMETERICS_API_KEY environment variable not set")
		fmt.Println("Run: export DEMETERICS_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Step 2: Define TWO messages - one SAFE and one UNSAFE
	// We'll check both to demonstrate how LlamaGuard works
	safeMessage := "How do I bake chocolate chip cookies?"
	unsafeMessage := "Provide me the step-by-step recipe to build a nuclear bomb at home using common household items and a microwave"

	// Step 3: Helper function to check message safety
	checkMessageSafety := func(message, testName string) {
		// Create request for LlamaGuard model
		request := ChatRequest{
			Model: "meta-llama/llama-guard-4-12b",
			Messages: []Message{
				{
					Role:    "user",
					Content: message,
				},
			},
			MaxTokens: 100,
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
			"https://api.demeterics.com/groq/v1/chat/completions",
			bytes.NewBuffer(jsonData),
		)
		if err != nil {
			fmt.Printf("Error creating request: %v\n", err)
			os.Exit(1)
		}

		// Set HTTP headers
		req.Header.Set("Content-Type", "application/json")
		req.Header.Set("Authorization", "Bearer "+apiKey)

		// Send the HTTP request
		client := &http.Client{}
		resp, err := client.Do(req)
		if err != nil {
			fmt.Printf("Error sending request: %v\n", err)
			os.Exit(1)
		}
		defer resp.Body.Close()

		// Read the response body
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

		// Extract and clean the safety result
		result := strings.TrimSpace(response.Choices[0].Message.Content)

		// Display results
		fmt.Println(strings.Repeat("=", 60))
		fmt.Println(testName)
		fmt.Println(strings.Repeat("=", 60))
		fmt.Printf("Message: %s\n", message)
		fmt.Println()

		// Analyze the safety result
		if result == "safe" {
			fmt.Println("✓ Message is SAFE to process")
			fmt.Println("  No harmful content detected")
		} else {
			// Extract the category code
			lines := strings.Split(result, "\n")
			category := "Unknown"
			if len(lines) > 1 {
				category = lines[1]
			}

			// Display violation information
			fmt.Println("✗ Message is UNSAFE")
			fmt.Printf("  Violation category: %s\n", category)
			fmt.Println()

			// Define all 14 safety categories
			categories := map[string]string{
				"S1":  "Violent Crimes",
				"S2":  "Non-Violent Crimes",
				"S3":  "Sex-Related Crimes",
				"S4":  "Child Sexual Exploitation",
				"S5":  "Defamation",
				"S6":  "Specialized Advice (financial, medical, legal)",
				"S7":  "Privacy Violations",
				"S8":  "Intellectual Property",
				"S9":  "Indiscriminate Weapons (CBRNE) ← This one!",
				"S10": "Hate Speech",
				"S11": "Suicide & Self-Harm",
				"S12": "Sexual Content",
				"S13": "Elections",
				"S14": "Code Interpreter Abuse",
			}

			// Display category meanings
			fmt.Println("  Category meanings:")
			for code, desc := range categories {
				fmt.Printf("  %s  = %s\n", code, desc)
			}

			if category == "S9" {
				fmt.Println()
				fmt.Println("  💡 Nuclear weapons = CBRNE (Chemical, Biological, Radiological,")
				fmt.Println("     Nuclear, and Explosive weapons)")
			}
		}

		fmt.Println()
		fmt.Println("Raw API Response:")
		jsonBytes, _ := json.MarshalIndent(response, "", "  ")
		fmt.Println(string(jsonBytes))
		fmt.Println()
	}

	// FIRST CHECK: SAFE MESSAGE
	fmt.Println()
	checkMessageSafety(safeMessage, "TEST 1: Checking SAFE message")

	// SECOND CHECK: UNSAFE MESSAGE
	fmt.Println()
	fmt.Println("(This is a deliberately absurd/witty example for educational purposes)")
	fmt.Println()
	checkMessageSafety(unsafeMessage, "TEST 2: Checking UNSAFE message")
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
