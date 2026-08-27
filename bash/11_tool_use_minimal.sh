#!/bin/bash
# Example 10: Tool Use with OpenRouter - AI Function Calling with Tavily
# Minimal version - demonstrates tool use flow with bash functions

# Check for required environment variables
if [ -z "$OPENROUTER_API_KEY" ] || [ -z "$TAVILY_API_KEY" ]; then
  echo "Error: Both OPENROUTER_API_KEY and TAVILY_API_KEY must be set"
  echo "Get an OpenRouter key: https://openrouter.ai/keys"
  echo "Get a Tavily key: https://tavily.com"
  exit 1
fi

# Tool function: Tavily Search
tavily_search() {
  local query="$1"
  curl -s -X POST "https://api.tavily.com/search" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TAVILY_API_KEY" \
    -d '{
      "query": "'"$query"'",
      "max_results": 3,
      "include_answer": true
    }'
}

# Tool function: Tavily Extract
tavily_extract() {
  local url="$1"
  curl -s -X POST "https://api.tavily.com/extract" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TAVILY_API_KEY" \
    -d '{
      "urls": ["'"$url"'"],
      "extract_depth": "basic"
    }'
}

# Step 1: Send initial request to OpenRouter with tool definitions
INITIAL_RESPONSE=$(curl -s -X POST "https://openrouter.ai/api/v1/chat/completions" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<'EOF'
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {
      "role": "user",
      "content": "Search for recent news about quantum computing and summarize the key developments"
    }
  ],
  "tools": [
    {
      "type": "function",
      "function": {
        "name": "tavily_search",
        "description": "Search the web for current information using Tavily API",
        "parameters": {
          "type": "object",
          "properties": {
            "query": {
              "type": "string",
              "description": "The search query"
            }
          },
          "required": ["query"]
        }
      }
    }
  ],
  "tool_choice": "auto"
}
EOF
)

# Step 2: Check if model wants to call a tool
TOOL_CALLS=$(echo "$INITIAL_RESPONSE" | jq -r '.choices[0].message.tool_calls // empty')

if [ -z "$TOOL_CALLS" ]; then
  # No tool call, just return the response
  echo "$INITIAL_RESPONSE" | jq -r '.choices[0].message.content'
  exit 0
fi

# Step 3: Execute the tool call
FUNCTION_NAME=$(echo "$INITIAL_RESPONSE" | jq -r '.choices[0].message.tool_calls[0].function.name')
ARGUMENTS=$(echo "$INITIAL_RESPONSE" | jq -r '.choices[0].message.tool_calls[0].function.arguments')
TOOL_CALL_ID=$(echo "$INITIAL_RESPONSE" | jq -r '.choices[0].message.tool_calls[0].id')

# Extract query argument
QUERY=$(echo "$ARGUMENTS" | jq -r '.query')

echo "AI wants to call: $FUNCTION_NAME with query: $QUERY"
echo ""

# Execute the bash function
TOOL_RESULT=$(tavily_search "$QUERY")

# Step 4: Send tool result back to OpenRouter for final response
# Build messages array properly with jq
MESSAGES=$(jq -n \
  --argjson assistant_msg "$(echo "$INITIAL_RESPONSE" | jq '.choices[0].message')" \
  --arg tool_call_id "$TOOL_CALL_ID" \
  --arg tool_result "$TOOL_RESULT" \
  '[
    {
      role: "user",
      content: "Search for recent news about quantum computing and summarize the key developments"
    },
    $assistant_msg,
    {
      role: "tool",
      tool_call_id: $tool_call_id,
      content: $tool_result
    }
  ]')

# tool_choice: "none" tells the model this is the final answer turn, not
# another chance to call a tool - without it, small models occasionally
# echo malformed tool-call syntax instead of a plain-text summary. Even
# with this, budget models can still occasionally return an empty/odd
# response on a multi-turn tool continuation - if that happens, just
# re-run; a production system would retry automatically.
FINAL_RESPONSE=$(curl -s -X POST "https://openrouter.ai/api/v1/chat/completions" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<EOF
{
  "model": "openai/gpt-oss-20b",
  "messages": $MESSAGES,
  "tool_choice": "none"
}
EOF
)

# Display final AI response
echo "AI Response:"
echo "$FINAL_RESPONSE" | jq -r '.choices[0].message.content'
