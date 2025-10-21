package main

/*
Example 11: Web Search with groq/compound-mini

What this demonstrates:
  - Calling a web-search-enabled model (groq/compound-mini)
  - OpenAI-compatible chat API
  - Inspecting optional reasoning and executed tool info

What you'll learn:
  - Building HTTP requests in Go (standard library)
  - Parsing optional JSON fields safely

Prerequisites:
  - GROQ_API_KEY environment variable set

Expected output:
  - Final answer content
  - Optional reasoning and executed tool info (if search was used)

Exercises: exercises/11_web_search.md
*/

import (
    "bytes"
    "encoding/json"
    "fmt"
    "io"
    "net/http"
    "os"
)

// Chat structures
type ChatMessage struct {
    Role    string `json:"role"`
    Content string `json:"content"`
}

type ChatRequest struct {
    Model       string        `json:"model"`
    Messages    []ChatMessage `json:"messages"`
    Temperature float64       `json:"temperature"`
    MaxTokens   int           `json:"max_tokens"`
}

// Partial response struct to extract what we need
type ChoiceMessage struct {
    Content       string      `json:"content"`
    Reasoning     string      `json:"reasoning,omitempty"`
    ExecutedTools interface{} `json:"executed_tools,omitempty"`
}

type Choice struct {
    Message ChoiceMessage `json:"message"`
}

type ChatResponse struct {
    Choices []Choice `json:"choices"`
}

func main() {
    apiKey := os.Getenv("GROQ_API_KEY")
    if apiKey == "" {
        fmt.Fprintln(os.Stderr, "Error: GROQ_API_KEY not set")
        fmt.Fprintln(os.Stderr, "Run: export GROQ_API_KEY=\"gsk_your_api_key_here\"")
        os.Exit(1)
    }

    reqBody := ChatRequest{
        Model: "groq/compound-mini",
        Messages: []ChatMessage{
            {Role: "system", Content: "You are a helpful research assistant. Provide concise answers with links. Use search when needed."},
            {Role: "user", Content: "What were the top 3 AI model releases last week? Include links and 1-sentence summaries."},
        },
        Temperature: 0.3,
        MaxTokens:   600,
    }

    b, err := json.Marshal(reqBody)
    if err != nil {
        fmt.Fprintf(os.Stderr, "marshal error: %v\n", err)
        os.Exit(1)
    }

    req, err := http.NewRequest("POST", "https://api.groq.com/openai/v1/chat/completions", bytes.NewBuffer(b))
    if err != nil {
        fmt.Fprintf(os.Stderr, "request error: %v\n", err)
        os.Exit(1)
    }
    req.Header.Set("Authorization", "Bearer "+apiKey)
    req.Header.Set("Content-Type", "application/json")

    resp, err := http.DefaultClient.Do(req)
    if err != nil {
        fmt.Fprintf(os.Stderr, "http error: %v\n", err)
        os.Exit(1)
    }
    defer resp.Body.Close()

    raw, err := io.ReadAll(resp.Body)
    if err != nil {
        fmt.Fprintf(os.Stderr, "read error: %v\n", err)
        os.Exit(1)
    }

    var out ChatResponse
    if err := json.Unmarshal(raw, &out); err != nil {
        fmt.Fprintf(os.Stderr, "parse error: %v\n", err)
        fmt.Fprintf(os.Stderr, "raw: %s\n", string(raw))
        os.Exit(1)
    }

    msg := ChoiceMessage{}
    if len(out.Choices) > 0 {
        msg = out.Choices[0].Message
    }

    fmt.Println("========================================")
    fmt.Println("Final Answer")
    fmt.Println("========================================")
    if msg.Content != "" {
        fmt.Println(msg.Content)
    } else {
        fmt.Println("(no content)")
    }
    fmt.Println()

    fmt.Println("========================================")
    fmt.Println("Optional Reasoning")
    fmt.Println("========================================")
    if msg.Reasoning != "" {
        fmt.Println(msg.Reasoning)
    } else {
        fmt.Println("(no reasoning)")
    }
    fmt.Println()

    fmt.Println("========================================")
    fmt.Println("Executed Tools (if any)")
    fmt.Println("========================================")
    if msg.ExecutedTools != nil {
        pretty, _ := json.MarshalIndent(msg.ExecutedTools, "", "  ")
        fmt.Println(string(pretty))
    } else {
        fmt.Println("(no executed tool info)")
    }
    fmt.Println()

    // Exercises:
    // 1) Change model to "groq/compound" for a larger model.
    // 2) Ask a boolean-style query: "(OpenAI OR Meta) AND release notes last 7 days".
    // 3) If your deployment supports web-search parameters (include/exclude domains, country),
    //    consult docs and add them in the payload.

    fmt.Println("Exercises: exercises/11_web_search.md")
}
