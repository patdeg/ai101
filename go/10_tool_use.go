package main

/*
Example 10: Tool Use (Function Calling) - AI Agents with Groq + Tavily

Demonstrates:
- Groq function calling with llama-4-scout
- Tavily Search and Extract as tools
- Multi-step agent workflow

Note: Uses Go standard library only
For production, consider using official SDKs
*/

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

// Tool function definitions
func tavilySearch(query string) (map[string]interface{}, error) {
	fmt.Printf("[Tool] Executing tavily_search with query: %s\n", query)

	requestBody, _ := json.Marshal(map[string]interface{}{
		"query":          query,
		"max_results":    5,
		"include_answer": true,
	})

	req, err := http.NewRequest("POST", "https://api.tavily.com/search", bytes.NewBuffer(requestBody))
	if err != nil {
		return nil, err
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %s", os.Getenv("TAVILY_API_KEY")))

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

func tavilyExtract(url string) (map[string]interface{}, error) {
	fmt.Printf("[Tool] Executing tavily_extract for URL: %s\n", url)

	requestBody, _ := json.Marshal(map[string]interface{}{
		"urls":          []string{url},
		"extract_depth": "basic",
	})

	req, err := http.NewRequest("POST", "https://api.tavily.com/extract", bytes.NewBuffer(requestBody))
	if err != nil {
		return nil, err
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %s", os.Getenv("TAVILY_API_KEY")))

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

func executeTool(functionName string, arguments map[string]interface{}) (map[string]interface{}, error) {
	switch functionName {
	case "tavily_search":
		return tavilySearch(arguments["query"].(string))
	case "tavily_extract":
		return tavilyExtract(arguments["url"].(string))
	default:
		return nil, fmt.Errorf("unknown function: %s", functionName)
	}
}

func callGroq(messages []interface{}, tools []interface{}) (map[string]interface{}, error) {
	requestData := map[string]interface{}{
		"model":    "meta-llama/llama-4-scout-17b-16e-instruct",
		"messages": messages,
	}

	if tools != nil {
		requestData["tools"] = tools
		requestData["tool_choice"] = "auto"
	}

	requestBody, _ := json.Marshal(requestData)

	req, err := http.NewRequest("POST", "https://api.groq.com/openai/v1/chat/completions", bytes.NewBuffer(requestBody))
	if err != nil {
		return nil, err
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %s", os.Getenv("GROQ_API_KEY")))

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
	// Check for API keys
	if os.Getenv("GROQ_API_KEY") == "" || os.Getenv("TAVILY_API_KEY") == "" {
		fmt.Fprintln(os.Stderr, "Error: Both GROQ_API_KEY and TAVILY_API_KEY must be set")
		fmt.Fprintln(os.Stderr, "Get Groq key: https://console.groq.com")
		fmt.Fprintln(os.Stderr, "Get Tavily key: https://tavily.com")
		os.Exit(1)
	}

	fmt.Println("========================================")
	fmt.Println("Groq Tool Use - AI Agent Demo")
	fmt.Println("========================================\n")

	userQuery := "Search for the latest developments in quantum computing and provide a summary"

	fmt.Printf("User Query: %s\n\n", userQuery)
	fmt.Println("Step 1: Sending query to AI with tool definitions...\n")

	// Tool definitions
	tools := []interface{}{
		map[string]interface{}{
			"type": "function",
			"function": map[string]interface{}{
				"name":        "tavily_search",
				"description": "Search the web for current information using Tavily API",
				"parameters": map[string]interface{}{
					"type": "object",
					"properties": map[string]interface{}{
						"query": map[string]interface{}{
							"type":        "string",
							"description": "The search query",
						},
					},
					"required": []string{"query"},
				},
			},
		},
		map[string]interface{}{
			"type": "function",
			"function": map[string]interface{}{
				"name":        "tavily_extract",
				"description": "Extract content from a specific URL",
				"parameters": map[string]interface{}{
					"type": "object",
					"properties": map[string]interface{}{
						"url": map[string]interface{}{
							"type":        "string",
							"description": "The URL to extract content from",
						},
					},
					"required": []string{"url"},
				},
			},
		},
	}

	// Step 1: Initial request
	initialResponse, err := callGroq([]interface{}{
		map[string]interface{}{"role": "user", "content": userQuery},
	}, tools)

	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

	// Check for tool calls
	choices := initialResponse["choices"].([]interface{})
	message := choices[0].(map[string]interface{})["message"].(map[string]interface{})
	toolCalls, hasToolCalls := message["tool_calls"]

	if !hasToolCalls || toolCalls == nil {
		fmt.Println("AI answered without using tools:\n")
		fmt.Println(message["content"])
		return
	}

	// Step 2: Execute tool calls
	fmt.Println("========================================")
	fmt.Println("AI decided to use tools!")
	fmt.Println("========================================\n")

	toolCallsList := toolCalls.([]interface{})
	fmt.Printf("Number of tool calls: %d\n\n", len(toolCallsList))

	var toolMessages []interface{}

	for _, toolCall := range toolCallsList {
		tc := toolCall.(map[string]interface{})
		function := tc["function"].(map[string]interface{})
		functionName := function["name"].(string)
		argumentsStr := function["arguments"].(string)

		fmt.Printf("Function: %s\n", functionName)
		fmt.Printf("Arguments: %s\n\n", argumentsStr)

		var arguments map[string]interface{}
		json.Unmarshal([]byte(argumentsStr), &arguments)

		result, err := executeTool(functionName, arguments)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error executing tool: %v\n", err)
			continue
		}

		resultJSON, _ := json.Marshal(result)

		toolMessages = append(toolMessages, map[string]interface{}{
			"role":         "tool",
			"tool_call_id": tc["id"],
			"content":      string(resultJSON),
		})

		fmt.Println("✓ Tool executed successfully\n")
	}

	// Step 3: Send results back to AI
	fmt.Println("Step 2: Sending tool results back to AI for final answer...\n")

	messages := []interface{}{
		map[string]interface{}{"role": "user", "content": userQuery},
		message,
	}
	messages = append(messages, toolMessages...)

	finalResponse, err := callGroq(messages, nil)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

	// Display final response
	fmt.Println("========================================")
	fmt.Println("Final AI Response")
	fmt.Println("========================================\n")

	finalChoices := finalResponse["choices"].([]interface{})
	finalMessage := finalChoices[0].(map[string]interface{})["message"].(map[string]interface{})
	fmt.Println(finalMessage["content"])
	fmt.Println()

	// Display token usage
	fmt.Println("========================================")
	fmt.Println("Token Usage")
	fmt.Println("========================================\n")

	usage := finalResponse["usage"].(map[string]interface{})
	promptTokens := usage["prompt_tokens"].(float64)
	completionTokens := usage["completion_tokens"].(float64)
	totalTokens := usage["total_tokens"].(float64)

	fmt.Printf("Prompt tokens: %.0f\n", promptTokens)
	fmt.Printf("Completion tokens: %.0f\n", completionTokens)
	fmt.Printf("Total tokens: %.0f\n", totalTokens)

	cost := (promptTokens*0.11 + completionTokens*0.34) / 1000000
	fmt.Printf("Cost: $%.6f\n\n", cost)

	fmt.Println("========================================")
	fmt.Println("Summary")
	fmt.Println("========================================\n")
	fmt.Println("✓ AI agent successfully used tools")
	fmt.Printf("✓ Executed %d tool call(s)\n", len(toolCallsList))
	fmt.Println("✓ Retrieved real-time information")
	fmt.Println("✓ Generated informed response\n")
}
