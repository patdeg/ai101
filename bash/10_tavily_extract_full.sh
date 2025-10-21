#!/bin/bash
################################################################################
# Example 09: Tavily Extract API - Extract Clean Content from Web Pages
# Full version with pretty output and parameter explanations
################################################################################
#
# What is Tavily Extract?
# ----------------------
# Tavily Extract converts web pages into clean, structured markdown:
# - Removes ads, navigation, footers, and clutter
# - Extracts main content in markdown format
# - Optionally extracts images with URLs
# - Supports basic and advanced extraction depths
# - Perfect for feeding content to LLMs
#
# Why use Tavily Extract?
# ----------------------
# - Clean content for AI processing (no HTML noise)
# - Faster than web scraping + parsing yourself
# - Handles JavaScript-heavy sites
# - Markdown output ready for LLMs
# - Batch processing (multiple URLs at once)
#
# This example:
# ------------
# - Extracts content from a Medium article about California AI rules
# - Uses "advanced" extraction depth for comprehensive content
# - Includes images from the article
# - Shows clean markdown output suitable for AI analysis
#
################################################################################

# Color codes for pretty output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check for required tools
if ! command -v curl > /dev/null 2>&1; then
  echo -e "${RED}Error: curl is required but not installed${NC}"
  exit 1
fi

if ! command -v jq > /dev/null 2>&1; then
  echo -e "${RED}Error: jq is required but not installed${NC}"
  echo "Install with: brew install jq (macOS) or sudo apt-get install jq (Linux)"
  exit 1
fi

# Check for API key
if [ -z "$TAVILY_API_KEY" ]; then
  echo -e "${RED}Error: TAVILY_API_KEY environment variable not set${NC}"
  echo ""
  echo "Get your free API key:"
  echo "  1. Visit https://tavily.com"
  echo "  2. Sign up for free"
  echo "  3. Copy your API key (starts with 'tvly-')"
  echo "  4. Set environment variable:"
  echo ""
  echo -e "     ${CYAN}export TAVILY_API_KEY='tvly-your-key-here'${NC}"
  echo ""
  exit 1
fi

################################################################################
# API Request Parameters Explained
################################################################################
#
# REQUIRED:
# ---------
# api_key        - Your Tavily API key
# urls           - Single URL string or array of URLs to extract
#
# OPTIONAL:
# ---------
# include_images - true/false - Extract image URLs (default: false)
# extract_depth  - "basic" or "advanced" (default: basic)
#                  - basic: faster, main content only
#                  - advanced: comprehensive, includes more context
# format         - "markdown" or "text" (default: markdown)
# timeout        - 1-60 seconds (optional)
#
# RESPONSE:
# ---------
# - results: Array of extracted content for each URL
#   - url: Original URL
#   - raw_content: Clean markdown/text
#   - images: Array of image URLs (if include_images: true)
# - failed_results: URLs that couldn't be processed
# - response_time: API response time
#
# PRICING:
# --------
# Free tier includes extract calls
# Paid tiers: Same as search pricing
#
################################################################################

# Tavily Extract API endpoint
API_URL="https://api.tavily.com/extract"

# Article URL to extract
ARTICLE_URL="https://medium.com/@pdeglon/california-ai-rules-explained-in-everyday-english-fea55637cb96"

echo -e "${BLUE}===========================================${NC}"
echo -e "${BLUE}Tavily Extract API - Medium Article${NC}"
echo -e "${BLUE}===========================================${NC}"
echo ""
echo -e "${YELLOW}URL:${NC} $ARTICLE_URL"
echo -e "${YELLOW}Options:${NC} extract_depth=advanced, include_images=true"
echo ""
echo -e "${CYAN}Sending request to Tavily...${NC}"
echo ""

# Make the API request
RESPONSE=$(curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d @- <<EOF
{
  "urls": ["$ARTICLE_URL"],
  "include_images": true,
  "extract_depth": "advanced"
}
EOF
)

# Check for errors
if echo "$RESPONSE" | jq -e '.error' > /dev/null 2>&1; then
  echo -e "${RED}API Error:${NC}"
  echo "$RESPONSE" | jq -r '.error'
  exit 1
fi

################################################################################
# Parse and Display Results
################################################################################

# Check if extraction succeeded
RESULT_COUNT=$(echo "$RESPONSE" | jq '.results | length')
FAILED_COUNT=$(echo "$RESPONSE" | jq '.failed_results | length')

if [ "$RESULT_COUNT" -eq 0 ]; then
  echo -e "${RED}Extraction failed${NC}"
  echo ""
  echo "Failed URLs:"
  echo "$RESPONSE" | jq -r '.failed_results[]'
  exit 1
fi

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Extraction Summary${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "${YELLOW}Successful:${NC} $RESULT_COUNT"
echo -e "${YELLOW}Failed:${NC} $FAILED_COUNT"
echo ""

# Extract content
CONTENT=$(echo "$RESPONSE" | jq -r '.results[0].raw_content')
CONTENT_LENGTH=$(echo "$CONTENT" | wc -c | tr -d ' ')

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Extracted Content (Markdown)${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "${YELLOW}Content length:${NC} $CONTENT_LENGTH characters"
echo ""
echo -e "${CYAN}First 1000 characters:${NC}"
echo ""
echo "$CONTENT" | head -c 1000
echo ""
echo -e "${YELLOW}... (truncated, see full output below) ...${NC}"
echo ""

# Extract images
IMAGE_COUNT=$(echo "$RESPONSE" | jq '.results[0].images | length')

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Extracted Images${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "${YELLOW}Found $IMAGE_COUNT images:${NC}"
echo ""

# Display image URLs
for i in $(seq 0 $((IMAGE_COUNT - 1))); do
  IMAGE_URL=$(echo "$RESPONSE" | jq -r ".results[0].images[$i]")
  echo -e "${BLUE}[$((i + 1))]${NC} $IMAGE_URL"
done
echo ""

# Display metadata
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Request Metadata${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

RESPONSE_TIME=$(echo "$RESPONSE" | jq -r '.response_time')

echo -e "${YELLOW}Response Time:${NC} ${RESPONSE_TIME}s"
echo -e "${YELLOW}Content Size:${NC} $CONTENT_LENGTH characters"
echo -e "${YELLOW}Images Extracted:${NC} $IMAGE_COUNT"
echo ""

################################################################################
# Save full content to file
################################################################################

OUTPUT_FILE="extracted_content.md"
echo "$CONTENT" > "$OUTPUT_FILE"

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Full Content Saved${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "${YELLOW}Saved to:${NC} $OUTPUT_FILE"
echo ""
echo -e "${CYAN}View with:${NC} cat $OUTPUT_FILE"
echo -e "${CYAN}Or open with:${NC} open $OUTPUT_FILE (macOS) / xdg-open $OUTPUT_FILE (Linux)"
echo ""

################################################################################
# Raw JSON Response (for debugging)
################################################################################

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Raw API Response (formatted)${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "$RESPONSE" | jq '.'
echo ""

################################################################################
# Summary
################################################################################

echo -e "${BLUE}===========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}===========================================${NC}"
echo ""
echo "✓ Content extracted successfully"
echo "✓ Extracted $CONTENT_LENGTH characters of clean markdown"
echo "✓ Found $IMAGE_COUNT images"
echo "✓ Saved to $OUTPUT_FILE"
echo "✓ Response time: ${RESPONSE_TIME}s"
echo ""
echo -e "${CYAN}Try the exercises to:${NC}"
echo "  - Extract content from multiple URLs at once"
echo "  - Compare basic vs. advanced extraction depth"
echo "  - Feed extracted content to AI for summarization"
echo "  - Build a web clipper / read-later service"
echo ""
