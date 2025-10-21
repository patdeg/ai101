#!/bin/sh
# Example 09: Tavily Extract API - Extract Clean Content from Web Pages
# Minimal version - raw JSON output, POSIX sh compatible

# Check for required environment variables
if [ -z "$TAVILY_API_KEY" ]; then
  echo "Error: TAVILY_API_KEY environment variable not set"
  echo "Get your free API key at: https://tavily.com"
  echo "Then run: export TAVILY_API_KEY='tvly-your-key-here'"
  exit 1
fi

# Tavily Extract API endpoint
API_URL="https://api.tavily.com/extract"

# Example: Extract content from Medium article about California AI rules
# Uses advanced depth for comprehensive extraction with images

curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d @- <<'EOF'
{
  "urls": ["https://medium.com/@pdeglon/california-ai-rules-explained-in-everyday-english-fea55637cb96"],
  "include_images": true,
  "extract_depth": "advanced"
}
EOF

# Exit cleanly
echo ""
