package main

/*
Example 08: Tavily Search API - Web Search with AI-Powered Answers

What is Tavily?
---------------
Tavily is a search API designed for AI agents and applications:
- Web search with AI-generated answer synthesis
- Topic-specific search (general, news, finance)
- Image search with AI descriptions
- Domain filtering (include/exclude)
- Time-restricted searches
- Clean, structured data (no ads)

Free tier: 1,000 searches/month
Get your API key at: https://tavily.com

This example:
------------
- Searches for "traditional French bread recipes"
- Requests AI-generated answer from multiple sources
- Returns 5 clean, relevant results
- No external dependencies (uses Go standard library)
*/

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

// TavilySearchRequest represents the API request structure
type TavilySearchRequest struct {
	Query         string `json:"query"`
	SearchDepth   string `json:"search_depth"`
	IncludeAnswer bool   `json:"include_answer"`
	MaxResults    int    `json:"max_results"`
	IncludeImages bool   `json:"include_images"`
}

// TavilySearchResponse represents the API response structure
type TavilySearchResponse struct {
	Answer       string `json:"answer"`
	ResponseTime float64 `json:"response_time"`
	RequestID    string `json:"request_id"`
	Results      []struct {
		Title   string  `json:"title"`
		URL     string  `json:"url"`
		Content string  `json:"content"`
		Score   float64 `json:"score"`
	} `json:"results"`
	Error string `json:"error,omitempty"`
}

func main() {
	// Check for API key
	apiKey := os.Getenv("TAVILY_API_KEY")
	if apiKey == "" {
		fmt.Fprintln(os.Stderr, "Error: TAVILY_API_KEY environment variable not set\n")
		fmt.Fprintln(os.Stderr, "Get your free API key:")
		fmt.Fprintln(os.Stderr, "  1. Visit https://tavily.com")
		fmt.Fprintln(os.Stderr, "  2. Sign up (1,000 free searches/month)")
		fmt.Fprintln(os.Stderr, "  3. Copy your API key (starts with \"tvly-\")")
		fmt.Fprintln(os.Stderr, "  4. Set: export TAVILY_API_KEY=\"tvly-your-key-here\"\n")
		os.Exit(1)
	}

	/*
		Tavily Search API Request Parameters

		REQUIRED:
		- query: Search query string
		- api_key: Your Tavily API key

		OPTIONAL:
		- search_depth: "basic" (default) or "advanced"
		- topic: "general" (default), "news", or "finance"
		- days: For news - days back to search (default: 7)
		- max_results: 1-20 results (default: 5)
		- include_answer: true/false/"basic"/"advanced"
		- include_images: Include relevant images
		- include_image_descriptions: AI descriptions of images
		- include_domains: Array of domains to include (max 300)
		- exclude_domains: Array of domains to exclude (max 150)
	*/

	request := TavilySearchRequest{
		Query:         "traditional French bread recipes",
		SearchDepth:   "basic",
		IncludeAnswer: true,
		MaxResults:    5,
		IncludeImages: false,
	}

	fmt.Println("===========================================")
	fmt.Println("Tavily Search API - French Bread Recipes")
	fmt.Println("===========================================\n")
	fmt.Println("Query: traditional French bread recipes")
	fmt.Println("Options: include_answer=true, max_results=5\n")
	fmt.Println("Sending request to Tavily...\n")

	// Make API request
	jsonData, err := json.Marshal(request)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error marshaling request: %v\n", err)
		os.Exit(1)
	}

	// Create request with Authorization header
	req, err := http.NewRequest("POST", "https://api.tavily.com/search", bytes.NewBuffer(jsonData))
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error creating request: %v\n", err)
		os.Exit(1)
	}
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %s", apiKey))

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error making request: %v\n", err)
		os.Exit(1)
	}
	defer resp.Body.Close()

	// Read response
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading response: %v\n", err)
		os.Exit(1)
	}

	// Parse response
	var result TavilySearchResponse
	if err := json.Unmarshal(body, &result); err != nil {
		fmt.Fprintf(os.Stderr, "Error parsing response: %v\n", err)
		fmt.Fprintf(os.Stderr, "Raw response: %s\n", string(body))
		os.Exit(1)
	}

	// Check for errors
	if result.Error != "" {
		fmt.Fprintf(os.Stderr, "API Error: %s\n", result.Error)
		os.Exit(1)
	}

	// Display AI-generated answer
	fmt.Println("========================================")
	fmt.Println("AI-Generated Answer (Answer Synthesis)")
	fmt.Println("========================================\n")
	if result.Answer != "" {
		fmt.Println(result.Answer)
	} else {
		fmt.Println("No answer provided")
	}
	fmt.Println()

	// Display search results
	fmt.Println("========================================")
	fmt.Println("Search Results (Top 5)")
	fmt.Println("========================================\n")
	fmt.Printf("Found %d results:\n\n", len(result.Results))

	for i, item := range result.Results {
		fmt.Printf("[%d] %s\n", i+1, item.Title)
		fmt.Printf("URL: %s\n", item.URL)
		fmt.Printf("Relevance Score: %.4f\n", item.Score)
		fmt.Printf("Summary: %s\n", item.Content)
		fmt.Println()
	}

	// Display metadata
	fmt.Println("========================================")
	fmt.Println("Request Metadata")
	fmt.Println("========================================\n")
	fmt.Printf("Response Time: %.2fs\n", result.ResponseTime)
	fmt.Printf("Request ID: %s\n", result.RequestID)
	fmt.Println()

	// Display raw JSON
	fmt.Println("========================================")
	fmt.Println("Raw API Response (formatted)")
	fmt.Println("========================================\n")
	prettyJSON, _ := json.MarshalIndent(result, "", "  ")
	fmt.Println(string(prettyJSON))
	fmt.Println()

	// Summary
	fmt.Println("===========================================")
	fmt.Println("Summary")
	fmt.Println("===========================================\n")
	fmt.Println("✓ Tavily Search completed successfully")
	fmt.Println("✓ AI Answer synthesized from multiple sources")
	fmt.Printf("✓ Returned %d relevant results\n", len(result.Results))
	fmt.Printf("✓ Response time: %.2fs\n\n", result.ResponseTime)
	fmt.Println("Try the exercises to explore:")
	fmt.Println("  - News search with time restrictions")
	fmt.Println("  - Finance search with domain filtering")
	fmt.Println("  - Image search with descriptions")
	fmt.Println("  - Building executive summaries with AI")
	fmt.Println("  - Sending results via ntfy.sh notifications\n")
}
