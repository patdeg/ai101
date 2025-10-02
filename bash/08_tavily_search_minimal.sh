#!/bin/sh
# Example 08: Tavily Search API - Web Search with AI-Powered Answers
# Minimal version - raw JSON output, POSIX sh compatible

# Check for required environment variables
if [ -z "$TAVILY_API_KEY" ]; then
  echo "Error: TAVILY_API_KEY environment variable not set"
  echo "Get your free API key at: https://tavily.com"
  echo "Then run: export TAVILY_API_KEY='tvly-your-key-here'"
  exit 1
fi

# Tavily API endpoint
API_URL="https://api.tavily.com/search"

# Example: Search for French bread recipes with AI answer
# This demonstrates the include_answer parameter which uses AI to synthesize search results

curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d @- <<'EOF'
{
  "query": "traditional French bread recipes",
  "search_depth": "basic",
  "include_answer": true,
  "max_results": 5
}
EOF

# Exit cleanly
echo ""
