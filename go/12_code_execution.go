package main

/*
Example 12: Code Execution with openai/gpt-oss-20b

What this demonstrates:
  - A model that can execute Python to solve tasks
  - Inspecting optional reasoning and executed tool information

What you'll learn:
  - Designing prompts that trigger code execution
  - Retrieving executed code and outputs (when provided)

Prerequisites:
  - GROQ_API_KEY set

Expected output:
  - Final answer content
  - message.reasoning and executed_tools (if present)

Exercises: exercises/12_code_execution.md
*/

import (
    "bytes"
    "encoding/json"
    "fmt"
    "io"
    "net/http"
    "os"
)

type Msg struct {
    Role    string `json:"role"`
    Content string `json:"content"`
}

type Req struct {
    Model       string `json:"model"`
    Messages    []Msg  `json:"messages"`
    Tools       []struct{
        Type string `json:"type"`
    } `json:"tools,omitempty"`
    ToolChoice  string  `json:"tool_choice,omitempty"`
    Temperature float64 `json:"temperature"`
    MaxTokens   int     `json:"max_tokens"`
}

type RespMessage struct {
    Content       string      `json:"content"`
    Reasoning     string      `json:"reasoning,omitempty"`
    ExecutedTools interface{} `json:"executed_tools,omitempty"`
}

type RespChoice struct {
    Message RespMessage `json:"message"`
}

type Resp struct {
    Choices []RespChoice `json:"choices"`
}

func main() {
    apiKey := os.Getenv("GROQ_API_KEY")
    if apiKey == "" {
        fmt.Fprintln(os.Stderr, "Error: GROQ_API_KEY not set")
        fmt.Fprintln(os.Stderr, "Run: export GROQ_API_KEY=\"gsk_your_api_key_here\"")
        os.Exit(1)
    }

    r := Req{
        Model: "openai/gpt-oss-20b",
        Messages: []Msg{
            {Role: "system", Content: "You can execute Python to verify results. If code raises an exception, show it and provide a corrected version."},
            {Role: "user", Content: "1) Compute the 2000th prime number using Python. 2) Execute Python: print(1/0) to demonstrate the exception, then show how to catch it without crashing."},
        },
        Tools: []struct{Type string `json:"type"`}{
            {Type: "code_interpreter"},
        },
        ToolChoice:  "required",
        Temperature: 0.2,
        MaxTokens:   900,
    }

    b, err := json.Marshal(r)
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

    var out Resp
    if err := json.Unmarshal(raw, &out); err != nil {
        fmt.Fprintf(os.Stderr, "parse error: %v\n", err)
        fmt.Fprintf(os.Stderr, "raw: %s\n", string(raw))
        os.Exit(1)
    }

    msg := RespMessage{}
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
    fmt.Println("Reasoning & Tool Calls (if any)")
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
    // 1) Switch to "openai/gpt-oss-120b" (if available) and compare.
    // 2) Ask to run code that may raise exceptions (invalid JSON) and show mitigation.
    // 3) Ask for the exact code executed and outputs.

    fmt.Println("Exercises: exercises/12_code_execution.md")
}
