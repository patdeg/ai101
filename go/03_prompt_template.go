package main

/*
03_prompt_template.go - Compile prompt templates for dynamic AI interactions

This example demonstrates advanced prompt engineering using templates:
1. Load template files with variables and conditional logic
2. Remove comment lines (///) for cleaner prompts
3. Substitute variables using Go's text/template package
4. Support conditional logic with custom delimiters [[]]
5. Generate dynamic, context-aware prompts

Template features:
- Variables: [[.VarName]] replaced with actual values
- Comments: Lines starting with /// are removed
- Conditionals: [[if eq .Category "X"]] ... [[end]]
- Timestamps: [[.Now]] replaced with current time

Usage:
    go run 03_prompt_template.go [category] [topic]

Examples:
    go run 03_prompt_template.go "Science" "Quantum Computing"
    go run 03_prompt_template.go "History" "The Industrial Revolution"
    go run 03_prompt_template.go "Technology" "Artificial Intelligence"

Environment:
    GROQ_API_KEY - Your Groq API key (required)
*/

import (
	"bufio"
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"text/template"
	"time"
)

// Configuration constants
const (
	API_URL       = "https://api.groq.com/openai/v1/chat/completions"
	MODEL         = "meta-llama/llama-4-scout-17b-16e-instruct"
	TEMPLATE_DIR  = "../templates"
	TEMPLATE_FILE = "essay_writer.txt"

	// Pricing per million tokens
	INPUT_PRICE  = 0.11
	OUTPUT_PRICE = 0.34
)

// ChatRequest represents the API request structure
type ChatRequest struct {
	Model       string    `json:"model"`
	Messages    []Message `json:"messages"`
	MaxTokens   int       `json:"max_tokens"`
	Temperature float32   `json:"temperature"`
}

// Message represents a chat message
type Message struct {
	Role    string `json:"role"`
	Content string `json:"content"`
}

// ChatResponse represents the API response
type ChatResponse struct {
	ID      string   `json:"id"`
	Object  string   `json:"object"`
	Created int64    `json:"created"`
	Model   string   `json:"model"`
	Choices []Choice `json:"choices"`
	Usage   Usage    `json:"usage"`
	Error   *Error   `json:"error,omitempty"`
}

// Choice represents a response choice
type Choice struct {
	Message      Message `json:"message"`
	Index        int     `json:"index"`
	FinishReason string  `json:"finish_reason"`
}

// Usage represents token usage statistics
type Usage struct {
	PromptTokens     int `json:"prompt_tokens"`
	CompletionTokens int `json:"completion_tokens"`
	TotalTokens      int `json:"total_tokens"`
}

// Error represents an API error
type Error struct {
	Message string `json:"message"`
	Type    string `json:"type"`
}

// TemplateData holds the data for template substitution
type TemplateData struct {
	Category string
	Topic    string
	Now      string
}

// createExampleTemplate creates a default template if none exists
func createExampleTemplate() error {
	templatePath := filepath.Join(TEMPLATE_DIR, TEMPLATE_FILE)

	// Create directory if it doesn't exist
	if err := os.MkdirAll(TEMPLATE_DIR, 0755); err != nil {
		return fmt.Errorf("failed to create template directory: %w", err)
	}

	templateContent := `/// Template for writing educational essays
/// Variables: [[.Now]] = current timestamp, [[.Category]] = essay category, [[.Topic]] = essay topic
/// Comments starting with /// are removed during compilation

# 🎯 OBJECTIVE

Write an engaging, educational essay about [[.Topic]] in the [[.Category]] category.
The essay should be appropriate for students aged 14-18, informative yet accessible.
Current timestamp: [[.Now]]

# 📝 ESSAY REQUIREMENTS

* **Length:** 500-750 words (approximately 5-7 paragraphs)
* **Structure:** Introduction, 3-4 body paragraphs, conclusion
* **Tone:** Educational, engaging, age-appropriate
* **Citations:** Include at least 3 factual references

# 📚 CATEGORY: [[.Category]]

[[if eq .Category "History"]]
Focus on:
- Cause and effect relationships
- Specific dates and key figures
- Historical significance and modern relevance
- Primary sources when possible
[[else if eq .Category "Science"]]
Focus on:
- Fundamental concepts explained simply
- Real-world applications
- Scientific method and evidence
- Recent discoveries or breakthroughs
[[else if eq .Category "Technology"]]
Focus on:
- Technical concepts in simple terms
- Benefits and challenges
- Future implications
- Ethical considerations
[[else]]
Focus on:
- Comprehensive overview
- Relevant examples
- Balanced perspective
- Critical analysis
[[end]]

# WRITING GUIDELINES

1. **Hook**: Start with an attention-grabbing opening
2. **Context**: Provide necessary background information
3. **Evidence**: Support claims with facts and examples
4. **Analysis**: Explain significance and connections
5. **Conclusion**: Synthesize key points and broader implications

# OUTPUT FORMAT

Provide the essay with:
- Clear, descriptive title
- Well-structured paragraphs with topic sentences
- Key terms defined in context
- 2-3 discussion questions at the end
- Suggestions for further reading

Remember to:
- Write for a teenage audience (14-18 years)
- Use clear, accessible language
- Include concrete examples
- Maintain academic integrity`

	// Write template to file
	if err := os.WriteFile(templatePath, []byte(templateContent), 0644); err != nil {
		return fmt.Errorf("failed to write template file: %w", err)
	}

	fmt.Printf("✅ Created template file: %s\n", templatePath)
	return nil
}

// removeComments removes lines starting with ///
func removeComments(content string) string {
	var result strings.Builder
	scanner := bufio.NewScanner(strings.NewReader(content))

	for scanner.Scan() {
		line := scanner.Text()
		// Skip lines that start with /// (after trimming whitespace)
		if !strings.HasPrefix(strings.TrimSpace(line), "///") {
			result.WriteString(line)
			result.WriteString("\n")
		}
	}

	return result.String()
}

// processTemplate loads and processes the template file
func processTemplate(data TemplateData) (string, error) {
	// Read template file
	templatePath := filepath.Join(TEMPLATE_DIR, TEMPLATE_FILE)
	content, err := os.ReadFile(templatePath)
	if err != nil {
		return "", fmt.Errorf("failed to read template: %w", err)
	}

	// Remove comment lines
	processedContent := removeComments(string(content))

	// Create template with custom delimiters [[ ]]
	tmpl, err := template.New("prompt").Delims("[[", "]]").Parse(processedContent)
	if err != nil {
		return "", fmt.Errorf("failed to parse template: %w", err)
	}

	// Execute template with data
	var buf bytes.Buffer
	if err := tmpl.Execute(&buf, data); err != nil {
		return "", fmt.Errorf("failed to execute template: %w", err)
	}

	return buf.String(), nil
}

// makeAPIRequest sends a request to the Groq API
func makeAPIRequest(systemPrompt, userPrompt, apiKey string) (*ChatResponse, error) {
	// Prepare request data
	request := ChatRequest{
		Model: MODEL,
		Messages: []Message{
			{Role: "system", Content: systemPrompt},
			{Role: "user", Content: userPrompt},
		},
		MaxTokens:   2000,
		Temperature: 0.7,
	}

	// Convert to JSON
	jsonData, err := json.Marshal(request)
	if err != nil {
		return nil, fmt.Errorf("failed to marshal request: %w", err)
	}

	// Create HTTP request
	req, err := http.NewRequest("POST", API_URL, bytes.NewBuffer(jsonData))
	if err != nil {
		return nil, fmt.Errorf("failed to create request: %w", err)
	}

	// Set headers
	req.Header.Set("Authorization", "Bearer "+apiKey)
	req.Header.Set("Content-Type", "application/json")

	// Make request
	client := &http.Client{Timeout: 30 * time.Second}
	resp, err := client.Do(req)
	if err != nil {
		return nil, fmt.Errorf("request failed: %w", err)
	}
	defer resp.Body.Close()

	// Read response
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("failed to read response: %w", err)
	}

	// Parse response
	var response ChatResponse
	if err := json.Unmarshal(body, &response); err != nil {
		return nil, fmt.Errorf("failed to parse response: %w", err)
	}

	// Check for API error
	if response.Error != nil {
		return nil, fmt.Errorf("API error: %s", response.Error.Message)
	}

	return &response, nil
}

// displayResponse shows the API response
func displayResponse(response *ChatResponse) {
	// Extract and display content
	if len(response.Choices) > 0 {
		content := response.Choices[0].Message.Content

		fmt.Println("\n" + strings.Repeat("=", 70))
		fmt.Println("GENERATED ESSAY")
		fmt.Println(strings.Repeat("=", 70))
		fmt.Println(content)
	} else {
		fmt.Println("No response generated")
	}

	// Display usage statistics
	if response.Usage.TotalTokens > 0 {
		usage := response.Usage

		// Calculate costs
		inputCost := float64(usage.PromptTokens) * INPUT_PRICE / 1_000_000
		outputCost := float64(usage.CompletionTokens) * OUTPUT_PRICE / 1_000_000
		totalCost := inputCost + outputCost

		fmt.Println("\n" + strings.Repeat("=", 70))
		fmt.Println("USAGE STATISTICS")
		fmt.Println(strings.Repeat("=", 70))
		fmt.Printf("Prompt tokens:     %d\n", usage.PromptTokens)
		fmt.Printf("Completion tokens: %d\n", usage.CompletionTokens)
		fmt.Printf("Total tokens:      %d\n", usage.TotalTokens)
		fmt.Printf("\nCost breakdown:\n")
		fmt.Printf("  Input:  $%.6f\n", inputCost)
		fmt.Printf("  Output: $%.6f\n", outputCost)
		fmt.Printf("  Total:  $%.6f\n", totalCost)
	}
}

func main() {
	// Parse command line arguments
	category := "Science"
	topic := "Climate Change"

	if len(os.Args) > 1 {
		category = os.Args[1]
	}
	if len(os.Args) > 2 {
		topic = os.Args[2]
	}

	fmt.Println(strings.Repeat("=", 70))
	fmt.Println("PROMPT TEMPLATE COMPILATION DEMO")
	fmt.Println(strings.Repeat("=", 70))
	fmt.Printf("Category: %s\n", category)
	fmt.Printf("Topic: %s\n", topic)

	// Check for API key
	apiKey := os.Getenv("GROQ_API_KEY")
	if apiKey == "" {
		fmt.Println("\n❌ Error: GROQ_API_KEY environment variable not set")
		fmt.Println("Set it with: export GROQ_API_KEY='your-api-key-here'")
		os.Exit(1)
	}

	// Check if template exists, create if not
	templatePath := filepath.Join(TEMPLATE_DIR, TEMPLATE_FILE)
	if _, err := os.Stat(templatePath); os.IsNotExist(err) {
		fmt.Printf("\n⚠️ Template not found at %s\n", templatePath)
		fmt.Println("Creating example template...")
		if err := createExampleTemplate(); err != nil {
			fmt.Printf("❌ Error creating template: %v\n", err)
			os.Exit(1)
		}
	}

	// Get current timestamp
	now := time.Now().UTC().Format("2006-01-02 15:04:05 UTC")
	fmt.Printf("Timestamp: %s\n", now)

	// Prepare template data
	data := TemplateData{
		Category: category,
		Topic:    topic,
		Now:      now,
	}

	// Process template
	fmt.Println("\n" + strings.Repeat("=", 70))
	fmt.Println("PROCESSING TEMPLATE")
	fmt.Println(strings.Repeat("=", 70))
	fmt.Printf("Loading template from: %s\n", templatePath)
	fmt.Println("Processing steps:")
	fmt.Println("  1. Removing comment lines (///)")
	fmt.Println("  2. Parsing template with custom delimiters [[]]")
	fmt.Println("  3. Substituting variables")
	fmt.Println("  4. Processing conditionals")

	processedPrompt, err := processTemplate(data)
	if err != nil {
		fmt.Printf("❌ Error processing template: %v\n", err)
		os.Exit(1)
	}

	// Show preview of processed prompt
	fmt.Println("\n" + strings.Repeat("=", 70))
	fmt.Println("COMPILED PROMPT (PREVIEW)")
	fmt.Println(strings.Repeat("=", 70))
	previewLength := 500
	if len(processedPrompt) < previewLength {
		previewLength = len(processedPrompt)
	}
	fmt.Printf("%s...\n", processedPrompt[:previewLength])
	fmt.Printf("\n(Showing first %d characters of %d total)\n", previewLength, len(processedPrompt))

	// Create user message
	userMessage := fmt.Sprintf("Please write the essay about %s as specified in the instructions.", topic)

	// Make API request
	fmt.Println("\n" + strings.Repeat("=", 70))
	fmt.Println("SENDING TO AI")
	fmt.Println(strings.Repeat("=", 70))
	fmt.Printf("Model: %s\n", MODEL)
	fmt.Println("Making API request...")

	response, err := makeAPIRequest(processedPrompt, userMessage, apiKey)
	if err != nil {
		fmt.Printf("❌ Error making API request: %v\n", err)
		os.Exit(1)
	}

	// Display response
	displayResponse(response)

	// Educational notes
	fmt.Println("\n" + strings.Repeat("=", 70))
	fmt.Println("💡 EDUCATIONAL NOTES")
	fmt.Println(strings.Repeat("=", 70))
	fmt.Println(`
Template-based prompting offers several advantages:

1. **Consistency**: Ensures uniform output format across requests
2. **Maintainability**: Templates can be updated without changing code
3. **Reusability**: Same template works for different inputs
4. **Clarity**: Separates prompt logic from application logic
5. **Version Control**: Templates can be tracked and versioned

Key techniques demonstrated:
- Variable substitution ([[.VarName]])
- Comment stripping (/// lines)
- Conditional logic (if/else blocks)
- Timestamp injection for context
- Category-specific instructions

Go-specific features used:
- text/template package with custom delimiters
- bufio.Scanner for line-by-line processing
- filepath for cross-platform path handling
- time.Format for timestamp generation
- Custom structs for API communication

The text/template package provides:
- Safe variable substitution
- Built-in functions (eq, ne, lt, etc.)
- Conditional logic support
- Custom delimiter support ([[]] instead of {{}})
	`)
}