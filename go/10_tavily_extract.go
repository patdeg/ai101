package main

/*
Example 09: Tavily Extract API - Extract Clean Content from Web Pages

What is Tavily Extract?
----------------------
Converts web pages into clean, structured markdown:
- Removes ads, navigation, and clutter
- Extracts main content in markdown format
- Optionally extracts images
- Perfect for feeding content to LLMs

This example:
------------
- Extracts content from a Medium article about California AI rules
- Uses "advanced" extraction depth
- Includes images from the article
- Saves clean markdown output to file
*/

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

// TavilyExtractRequest represents the API request structure
type TavilyExtractRequest struct {
	URLs          []string `json:"urls"`
	IncludeImages bool     `json:"include_images"`
	ExtractDepth  string   `json:"extract_depth"`
}

// TavilyExtractResponse represents the API response structure
type TavilyExtractResponse struct {
	Results []struct {
		URL        string   `json:"url"`
		RawContent string   `json:"raw_content"`
		Images     []string `json:"images"`
	} `json:"results"`
	FailedResults []string `json:"failed_results"`
	ResponseTime  float64  `json:"response_time"`
	Error         string   `json:"error,omitempty"`
}

func main() {
	// Check for API key
	apiKey := os.Getenv("TAVILY_API_KEY")
	if apiKey == "" {
		fmt.Fprintln(os.Stderr, "Error: TAVILY_API_KEY environment variable not set\n")
		fmt.Fprintln(os.Stderr, "Get your free API key at: https://tavily.com")
		fmt.Fprintln(os.Stderr, "Then: export TAVILY_API_KEY=\"tvly-your-key-here\"\n")
		os.Exit(1)
	}

	articleURL := "https://medium.com/@pdeglon/california-ai-rules-explained-in-everyday-english-fea55637cb96"

	request := TavilyExtractRequest{
		URLs:          []string{articleURL},
		IncludeImages: true,
		ExtractDepth:  "advanced",
	}

	fmt.Println("===========================================")
	fmt.Println("Tavily Extract API - Medium Article")
	fmt.Println("===========================================\n")
	fmt.Printf("URL: %s\n", articleURL)
	fmt.Println("Options: extract_depth=advanced, include_images=true\n")
	fmt.Println("Sending request to Tavily...\n")

	// Make API request
	jsonData, err := json.Marshal(request)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error marshaling request: %v\n", err)
		os.Exit(1)
	}

	// Create request with Authorization header
	req, err := http.NewRequest("POST", "https://api.tavily.com/extract", bytes.NewBuffer(jsonData))
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
	var result TavilyExtractResponse
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

	// Check extraction results
	resultCount := len(result.Results)
	failedCount := len(result.FailedResults)

	if resultCount == 0 {
		fmt.Fprintln(os.Stderr, "Extraction failed\n")
		fmt.Fprintf(os.Stderr, "Failed URLs: %v\n", result.FailedResults)
		os.Exit(1)
	}

	fmt.Println("========================================")
	fmt.Println("Extraction Summary")
	fmt.Println("========================================\n")
	fmt.Printf("Successful: %d\n", resultCount)
	fmt.Printf("Failed: %d\n\n", failedCount)

	// Extract content
	content := result.Results[0].RawContent
	contentLength := len(content)

	fmt.Println("========================================")
	fmt.Println("Extracted Content (Markdown)")
	fmt.Println("========================================\n")
	fmt.Printf("Content length: %d characters\n\n", contentLength)
	fmt.Println("First 1000 characters:\n")
	if contentLength > 1000 {
		fmt.Println(content[:1000])
		fmt.Println("\n... (truncated, see full output below) ...\n")
	} else {
		fmt.Println(content)
		fmt.Println()
	}

	// Extract images
	images := result.Results[0].Images
	imageCount := len(images)

	fmt.Println("========================================")
	fmt.Println("Extracted Images")
	fmt.Println("========================================\n")
	fmt.Printf("Found %d images:\n\n", imageCount)

	for i, imageURL := range images {
		fmt.Printf("[%d] %s\n", i+1, imageURL)
	}
	fmt.Println()

	// Display metadata
	fmt.Println("========================================")
	fmt.Println("Request Metadata")
	fmt.Println("========================================\n")
	fmt.Printf("Response Time: %.2fs\n", result.ResponseTime)
	fmt.Printf("Content Size: %d characters\n", contentLength)
	fmt.Printf("Images Extracted: %d\n\n", imageCount)

	// Save full content to file
	outputFile := "extracted_content.md"
	err = os.WriteFile(outputFile, []byte(content), 0644)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error saving file: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("========================================")
	fmt.Println("Full Content Saved")
	fmt.Println("========================================\n")
	fmt.Printf("Saved to: %s\n\n", outputFile)
	fmt.Printf("View with: cat %s\n", outputFile)
	fmt.Println("Or open with your editor\n")

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
	fmt.Println("✓ Content extracted successfully")
	fmt.Printf("✓ Extracted %d characters of clean markdown\n", contentLength)
	fmt.Printf("✓ Found %d images\n", imageCount)
	fmt.Printf("✓ Saved to %s\n", outputFile)
	fmt.Printf("✓ Response time: %.2fs\n\n", result.ResponseTime)
	fmt.Println("Try the exercises to:")
	fmt.Println("  - Extract content from multiple URLs at once")
	fmt.Println("  - Compare basic vs. advanced extraction depth")
	fmt.Println("  - Feed extracted content to AI for summarization")
	fmt.Println("  - Build a web clipper / read-later service\n")
}
