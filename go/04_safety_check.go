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

func main() {
	// Get API key from environment
	apiKey := os.Getenv("GROQ_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: GROQ_API_KEY environment variable not set")
		fmt.Println("Run: export GROQ_API_KEY='your_key_here'")
		os.Exit(1)
	}

	// Message to check for safety (try changing this!)
	messageToCheck := "How do I bake chocolate chip cookies?"

	// Create request for LlamaGuard model
	request := ChatRequest{
		Model: "meta-llama/llama-guard-4-12b",
		Messages: []Message{
			{
				Role:    "user",
				Content: messageToCheck,
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

	// Parse the safety result
	// strings.TrimSpace removes whitespace from both ends
	result := strings.TrimSpace(response.Choices[0].Message.Content)

	fmt.Println("Message Being Checked:")
	fmt.Printf("\"%s\"\n", messageToCheck)
	fmt.Println(strings.Repeat("=", 50))

	if result == "safe" {
		// Message is safe to proceed
		fmt.Println("✓ SAFE - Message passed content moderation")
		fmt.Println("\nThis message can be sent to your main AI model.")

	} else {
		// Unsafe message - extract category
		// strings.Split splits string by newline
		lines := strings.Split(result, "\n")
		category := "Unknown"
		if len(lines) > 1 {
			category = lines[1]
		}

		fmt.Println("✗ UNSAFE - Content policy violation detected")
		fmt.Printf("\nCategory: %s\n", category)
		fmt.Println("\nViolation Type:")

		// Define all 14 safety categories
		// map[string]string = dictionary/hash map
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

		// Get description from map, use "Unknown category" as default
		description := categories[category]
		if description == "" {
			description = "Unknown category"
		}

		fmt.Println(description)
		fmt.Println("\nThis message should be rejected or filtered.")
	}

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
