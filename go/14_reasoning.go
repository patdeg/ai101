package main

/*
Example 13: Reasoning with openai/gpt-oss-20b

Demonstrates:
- Reasoning models that show step-by-step thinking
- Three reasoning formats: raw, parsed, hidden
- Three reasoning effort levels: low, medium, high
- Cache optimization for cost savings

What you'll learn:
- How reasoning models differ from chat models
- When to use different reasoning formats
- How to optimize message order for cache hits
- Token usage and cost tracking

Note: Uses Go standard library only
*/

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

func callReasoning(messages []interface{}, formatType *string, effort string) (map[string]interface{}, error) {
	requestData := map[string]interface{}{
		"model":                 "openai/gpt-oss-20b",
		"messages":              messages,
		"temperature":           0.6,
		"max_completion_tokens": 1024,
		"reasoning_effort":      effort,
	}

	if formatType != nil {
		requestData["reasoning_format"] = *formatType
	}

	requestBody, _ := json.Marshal(requestData)

	req, err := http.NewRequest("POST", "https://api.demeterics.com/groq/v1/chat/completions", bytes.NewBuffer(requestBody))
	if err != nil {
		return nil, err
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %s", os.Getenv("DEMETERICS_API_KEY")))

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	body, _ := io.ReadAll(resp.Body)

	var result map[string]interface{}
	json.Unmarshal(body, &result)
	return result, nil
}

func main() {
	// Check for API key
	if os.Getenv("DEMETERICS_API_KEY") == "" {
		fmt.Fprintln(os.Stderr, "Error: DEMETERICS_API_KEY not set")
		fmt.Fprintln(os.Stderr, "Get your Managed LLM Key from: https://demeterics.com")
		os.Exit(1)
	}

	fmt.Println("========================================")
	fmt.Println("Reasoning with openai/gpt-oss-20b")
	fmt.Println("========================================\n")

	query := "How many 'r' letters are in the word 'strawberry'? Think through this step-by-step."
	messages := []interface{}{
		map[string]interface{}{"role": "user", "content": query},
	}

	fmt.Printf("Query: %s\n\n", query)

	// Demo 1: Raw format (default)
	fmt.Println("========================================")
	fmt.Println("Demo 1: Raw Format (reasoning in <think> tags)")
	fmt.Println("========================================\n")

	rawResponse, err := callReasoning(messages, nil, "medium")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

	choices := rawResponse["choices"].([]interface{})
	message := choices[0].(map[string]interface{})["message"].(map[string]interface{})
	content := message["content"].(string)

	fmt.Println("Response (raw format):")
	fmt.Println(content)
	fmt.Println()

	usage := rawResponse["usage"].(map[string]interface{})
	promptTokens := usage["prompt_tokens"].(float64)
	completionTokens := usage["completion_tokens"].(float64)
	totalTokens := usage["total_tokens"].(float64)

	fmt.Println("Token Usage (Demo 1):")
	fmt.Printf("  Prompt tokens: %.0f\n", promptTokens)
	fmt.Printf("  Completion tokens: %.0f\n", completionTokens)
	fmt.Printf("  Total tokens: %.0f\n\n", totalTokens)

	// Demo 2: Parsed format
	fmt.Println("========================================")
	fmt.Println("Demo 2: Parsed Format (separate reasoning field)")
	fmt.Println("========================================\n")

	parsedFormat := "parsed"
	parsedResponse, _ := callReasoning(messages, &parsedFormat, "medium")

	parsedChoices := parsedResponse["choices"].([]interface{})
	parsedMessage := parsedChoices[0].(map[string]interface{})["message"].(map[string]interface{})

	if reasoning, ok := parsedMessage["reasoning"].(string); ok {
		fmt.Println("Reasoning process:")
		fmt.Println(reasoning)
		fmt.Println()
	}

	fmt.Println("Final answer:")
	fmt.Println(parsedMessage["content"].(string))
	fmt.Println()

	// Demo 3: Hidden format
	fmt.Println("========================================")
	fmt.Println("Demo 3: Hidden Format (only final answer)")
	fmt.Println("========================================\n")

	hiddenFormat := "hidden"
	hiddenResponse, _ := callReasoning(messages, &hiddenFormat, "medium")

	hiddenChoices := hiddenResponse["choices"].([]interface{})
	hiddenMessage := hiddenChoices[0].(map[string]interface{})["message"].(map[string]interface{})

	fmt.Println("Response (hidden format):")
	fmt.Println(hiddenMessage["content"].(string))
	fmt.Println()

	// Cost calculation
	fmt.Println("========================================")
	fmt.Println("Cost Analysis")
	fmt.Println("========================================\n")

	inputCostPer1M := 0.10
	cachedInputCostPer1M := 0.05
	outputCostPer1M := 0.50

	inputCost := (promptTokens * inputCostPer1M) / 1000000
	outputCost := (completionTokens * outputCostPer1M) / 1000000
	totalCost := inputCost + outputCost

	fmt.Println("Pricing (openai/gpt-oss-20b):")
	fmt.Println("  Input tokens: $0.10 per 1M")
	fmt.Println("  Cached input tokens: $0.05 per 1M (50% discount!)")
	fmt.Println("  Output tokens: $0.50 per 1M\n")

	fmt.Println("This request:")
	fmt.Printf("  Input cost: $%.6f (%.0f tokens)\n", inputCost, promptTokens)
	fmt.Printf("  Output cost: $%.6f (%.0f tokens)\n", outputCost, completionTokens)
	fmt.Printf("  Total cost: $%.6f\n\n", totalCost)

	// Cache optimization explanation
	fmt.Println("========================================")
	fmt.Println("Cache Optimization Strategy")
	fmt.Println("========================================\n")

	fmt.Println("What is cached input?")
	fmt.Println("Groq caches parts of your prompt to speed up responses and reduce costs.")
	fmt.Println("Cached tokens are 50% cheaper ($0.05/1M vs $0.10/1M for this model).\n")

	fmt.Println("How to maximize cache hits:")
	fmt.Println("Order messages from most constant to most variable:\n")
	fmt.Println("  1. System prompts (always the same across requests)")
	fmt.Println("  2. Context/documents (same during a conversation)")
	fmt.Println("  3. User questions (changes with each request)\n")

	fmt.Println("Example: agentresume.ai recruiter chatbot\n")
	fmt.Println("Optimal message order:")
	fmt.Println(`[
  {
    "role": "system",
    "content": "You are a recruiter assistant. Analyze resumes..."
  },                                                    ← Always cached
  {
    "role": "user",
    "content": "Candidate Resume:\nJohn Doe\nSkills: Python, Go..."
  },                                                    ← Cached per conversation
  {
    "role": "user",
    "content": "Does this candidate have Python experience?"
  }                                                     ← Not cached (changes each Q)
]
`)

	fmt.Println("Cache benefit calculation:")
	fmt.Println("If 1000 tokens are always cached (system + resume):")
	fmt.Println("  Normal cost: 1000 × $0.10 / 1M = $0.0001")
	fmt.Println("  Cached cost: 1000 × $0.05 / 1M = $0.00005")
	fmt.Println("  Savings: 50% per request!\n")

	fmt.Println("========================================")
	fmt.Println("Reasoning Effort Comparison")
	fmt.Println("========================================\n")

	fmt.Println("Reasoning effort levels:")
	fmt.Println("  low    - Quick analysis, fewer reasoning tokens")
	fmt.Println("  medium - Balanced reasoning (default)")
	fmt.Println("  high   - Deep analysis, more reasoning tokens\n")
	fmt.Println("Use 'low' for simple questions, 'high' for complex reasoning tasks.\n")

	fmt.Println("========================================")
	fmt.Println("Summary")
	fmt.Println("========================================\n")
	fmt.Println("✓ Demonstrated three reasoning formats")
	fmt.Println("✓ Showed token usage and costs")
	fmt.Println("✓ Explained cache optimization strategy")
	fmt.Println("✓ Model excels at step-by-step logical reasoning\n")
}
