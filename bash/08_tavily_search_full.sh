#!/bin/bash
################################################################################
# Example 08: Tavily Search API - Web Search with AI-Powered Answers
# Full version with pretty output, parameter explanations, and cost tracking
################################################################################
#
# What is Tavily?
# ---------------
# Tavily is a search API designed for AI agents and applications. It provides:
# - Web search with AI-generated answers
# - News and finance-specific search
# - Image search and descriptions
# - Domain filtering (include/exclude specific sites)
# - Time-restricted searches (for news)
#
# Why use Tavily?
# ---------------
# - Purpose-built for AI/LLM applications
# - Returns clean, structured data (no ads, clutter)
# - AI-powered answer synthesis from multiple sources
# - Topic-specific search (general, news, finance)
# - Free tier available: 1,000 searches/month
#
# Get your API key:
# ----------------
# 1. Visit https://tavily.com
# 2. Sign up for free account
# 3. Get your API key (starts with 'tvly-')
# 4. Set environment variable: export TAVILY_API_KEY='tvly-your-key-here'
#
# This example:
# ------------
# - Searches for "traditional French bread recipes"
# - Requests AI-generated answer (include_answer: true)
# - Gets 5 search results with clean formatting
# - Shows how to parse and display results
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
  echo "  2. Sign up for free (1,000 searches/month)"
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
# query          - The search query (string)
# api_key        - Your Tavily API key
#
# OPTIONAL:
# ---------
# search_depth   - "basic" (default) or "advanced" (more comprehensive)
# topic          - "general" (default), "news", or "finance"
# days           - For news topic: how many days back to search (default: 7)
# max_results    - Number of results (1-20, default: 5)
# include_answer - true/false/"basic"/"advanced" - AI-generated answer
# include_images - true/false - Include relevant images
# include_image_descriptions - true/false - Describe images with AI
# include_domains - Array of domains to include (max 300)
# exclude_domains - Array of domains to exclude (max 150)
#
# PRICING:
# --------
# Free tier: 1,000 searches/month
# Paid tiers start at $100/month for 10,000 searches = $0.01 per search
#
################################################################################

# Tavily API endpoint
API_URL="https://api.tavily.com/search"

echo -e "${BLUE}===========================================

="${NC}"
echo -e "${BLUE}Tavily Search API - French Bread Recipes${NC}"
echo -e "${BLUE}===========================================${NC}"
echo ""
echo -e "${YELLOW}Query:${NC} traditional French bread recipes"
echo -e "${YELLOW}Options:${NC} include_answer=true, max_results=5"
echo ""
echo -e "${CYAN}Sending request to Tavily...${NC}"
echo ""

# Make the API request
RESPONSE=$(curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d @- <<EOF
{
  "query": "traditional French bread recipes",
  "search_depth": "basic",
  "include_answer": true,
  "max_results": 5,
  "include_images": false
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

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}AI-Generated Answer (Answer Synthesis)${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Extract and display AI answer
ANSWER=$(echo "$RESPONSE" | jq -r '.answer // "No answer provided"')
echo -e "${CYAN}$ANSWER${NC}"
echo ""

# Display search results
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Search Results (Top 5)${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Count results
RESULT_COUNT=$(echo "$RESPONSE" | jq '.results | length')
echo -e "${YELLOW}Found $RESULT_COUNT results:${NC}"
echo ""

# Loop through results
for i in $(seq 0 $((RESULT_COUNT - 1))); do
  TITLE=$(echo "$RESPONSE" | jq -r ".results[$i].title")
  URL=$(echo "$RESPONSE" | jq -r ".results[$i].url")
  SNIPPET=$(echo "$RESPONSE" | jq -r ".results[$i].content")
  SCORE=$(echo "$RESPONSE" | jq -r ".results[$i].score")

  echo -e "${BLUE}[$((i + 1))] $TITLE${NC}"
  echo -e "${YELLOW}URL:${NC} $URL"
  echo -e "${YELLOW}Relevance Score:${NC} $SCORE"
  echo -e "${CYAN}Summary:${NC} $SNIPPET"
  echo ""
done

# Display metadata
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Request Metadata${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

RESPONSE_TIME=$(echo "$RESPONSE" | jq -r '.response_time')
REQUEST_ID=$(echo "$RESPONSE" | jq -r '.request_id // "N/A"')

echo -e "${YELLOW}Response Time:${NC} ${RESPONSE_TIME}s"
echo -e "${YELLOW}Request ID:${NC} $REQUEST_ID"
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
echo "✓ Tavily Search completed successfully"
echo "✓ AI Answer synthesized from multiple sources"
echo "✓ Returned $RESULT_COUNT relevant results"
echo "✓ Response time: ${RESPONSE_TIME}s"
echo ""
echo -e "${CYAN}Try the exercises to explore:${NC}"
echo "  - News search with time restrictions"
echo "  - Finance search with domain filtering"
echo "  - Image search with descriptions"
echo "  - Building executive summaries with AI"
echo "  - Sending results via ntfy.sh notifications"
echo ""
