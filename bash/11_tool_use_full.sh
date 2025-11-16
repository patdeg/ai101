#!/bin/bash
################################################################################
# Example 10: Tool Use (Function Calling) - AI Agents with Groq + Tavily
# Full version with explanations and multiple tool support
################################################################################
#
# What is Tool Use / Function Calling?
# ------------------------------------
# Tool use allows AI models to:
# 1. Decide when they need external data or capabilities
# 2. Call functions/APIs to get that information
# 3. Use the results to provide informed answers
#
# This creates AI "agents" that can:
# - Search the web for current information
# - Extract content from URLs
# - Perform calculations
# - Query databases
# - Execute any function you define
#
# How it works:
# ------------
# 1. You define available tools (functions) with descriptions
# 2. Send user query + tool definitions to AI
# 3. AI decides if it needs to call a tool
# 4. You execute the tool and return results
# 5. Send results back to AI for final response
#
# This example demonstrates:
# -------------------------
# - Defining tools as bash functions
# - Groq function calling with llama-4-scout
# - Tavily Search and Extract as tools
# - Multi-step agent workflow
#
################################################################################

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

# Check for required tools
if ! command -v jq > /dev/null 2>&1; then
  echo -e "${RED}Error: jq is required${NC}"
  exit 1
fi

# Check for API keys
if [ -z "$DEMETERICS_API_KEY" ]; then
  echo -e "${RED}Error: DEMETERICS_API_KEY not set${NC}"
  exit 1
fi

if [ -z "$TAVILY_API_KEY" ]; then
  echo -e "${RED}Error: TAVILY_API_KEY not set${NC}"
  exit 1
fi

################################################################################
# Tool Definitions (Bash Functions)
################################################################################

# Tool 1: Search the web with Tavily
tavily_search() {
  local query="$1"
  echo -e "${CYAN}[Tool] Executing tavily_search with query: $query${NC}" >&2

  curl -s -X POST "https://api.tavily.com/search" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TAVILY_API_KEY" \
    -d '{
      "query": "'"$query"'",
      "max_results": 5,
      "include_answer": true,
      "search_depth": "basic"
    }'
}

# Tool 2: Extract content from a URL
tavily_extract() {
  local url="$1"
  echo -e "${CYAN}[Tool] Executing tavily_extract for URL: $url${NC}" >&2

  curl -s -X POST "https://api.tavily.com/extract" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TAVILY_API_KEY" \
    -d '{
      "urls": ["'"$url"'"],
      "extract_depth": "basic"
    }'
}

################################################################################
# Tool Execution Router
################################################################################

execute_tool() {
  local function_name="$1"
  local arguments="$2"

  case "$function_name" in
    "tavily_search")
      local query=$(echo "$arguments" | jq -r '.query')
      tavily_search "$query"
      ;;
    "tavily_extract")
      local url=$(echo "$arguments" | jq -r '.url')
      tavily_extract "$url"
      ;;
    *)
      echo '{"error": "Unknown function: '"$function_name"'"}'
      ;;
  esac
}

################################################################################
# Main Tool Use Flow
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Groq Tool Use - AI Agent Demo${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# User query
USER_QUERY="Search for the latest developments in quantum computing and provide a summary"

echo -e "${YELLOW}User Query:${NC} $USER_QUERY"
echo ""
echo -e "${CYAN}Step 1: Sending query to AI with tool definitions...${NC}"
echo ""

# Step 1: Initial request with tool definitions
INITIAL_RESPONSE=$(curl -s -X POST "https://api.demeterics.com/groq/v1/chat/completions" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<EOF
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
  "messages": [
    {
      "role": "user",
      "content": "$USER_QUERY"
    }
  ],
  "tools": [
    {
      "type": "function",
      "function": {
        "name": "tavily_search",
        "description": "Search the web for current information, news, and real-time data using Tavily API. Use this when you need up-to-date information.",
        "parameters": {
          "type": "object",
          "properties": {
            "query": {
              "type": "string",
              "description": "The search query to find information about"
            }
          },
          "required": ["query"]
        }
      }
    },
    {
      "type": "function",
      "function": {
        "name": "tavily_extract",
        "description": "Extract clean content from a specific URL. Use this to get the full text of an article or webpage.",
        "parameters": {
          "type": "object",
          "properties": {
            "url": {
              "type": "string",
              "description": "The URL to extract content from"
            }
          },
          "required": ["url"]
        }
      }
    }
  ],
  "tool_choice": "auto"
}
EOF
)

# Check for errors
if echo "$INITIAL_RESPONSE" | jq -e '.error' > /dev/null 2>&1; then
  echo -e "${RED}API Error:${NC}"
  echo "$INITIAL_RESPONSE" | jq -r '.error.message'
  exit 1
fi

# Step 2: Check if AI wants to use tools
TOOL_CALLS=$(echo "$INITIAL_RESPONSE" | jq -r '.choices[0].message.tool_calls // empty')

if [ -z "$TOOL_CALLS" ] || [ "$TOOL_CALLS" = "null" ]; then
  # No tool calls - AI answered directly
  echo -e "${YELLOW}AI answered without using tools:${NC}"
  echo ""
  echo "$INITIAL_RESPONSE" | jq -r '.choices[0].message.content'
  exit 0
fi

# Step 3: Execute tool calls
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}AI decided to use tools!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Count tool calls
TOOL_CALL_COUNT=$(echo "$INITIAL_RESPONSE" | jq '.choices[0].message.tool_calls | length')
echo -e "${YELLOW}Number of tool calls:${NC} $TOOL_CALL_COUNT"
echo ""

# Build tool messages array
TOOL_MESSAGES="[]"

for i in $(seq 0 $((TOOL_CALL_COUNT - 1))); do
  TOOL_CALL=$(echo "$INITIAL_RESPONSE" | jq ".choices[0].message.tool_calls[$i]")
  TOOL_CALL_ID=$(echo "$TOOL_CALL" | jq -r '.id')
  FUNCTION_NAME=$(echo "$TOOL_CALL" | jq -r '.function.name')
  ARGUMENTS=$(echo "$TOOL_CALL" | jq -r '.function.arguments')

  echo -e "${BLUE}Tool Call $((i + 1)):${NC}"
  echo -e "  Function: $FUNCTION_NAME"
  echo -e "  Arguments: $ARGUMENTS"
  echo ""

  # Execute the tool
  TOOL_RESULT=$(execute_tool "$FUNCTION_NAME" "$ARGUMENTS")

  # Add to tool messages
  TOOL_MESSAGE=$(jq -n \
    --arg id "$TOOL_CALL_ID" \
    --arg content "$TOOL_RESULT" \
    '{
      role: "tool",
      tool_call_id: $id,
      content: $content
    }')

  TOOL_MESSAGES=$(echo "$TOOL_MESSAGES" | jq ". += [$TOOL_MESSAGE]")

  echo -e "${GREEN}✓ Tool executed successfully${NC}"
  echo ""
done

# Step 4: Send tool results back to AI
echo -e "${CYAN}Step 2: Sending tool results back to AI for final answer...${NC}"
echo ""

# Build messages array properly
MESSAGES_JSON=$(jq -n \
  --arg user_query "$USER_QUERY" \
  --argjson assistant "$(echo "$INITIAL_RESPONSE" | jq '.choices[0].message')" \
  --argjson tool_msgs "$TOOL_MESSAGES" \
  '[
    {role: "user", content: $user_query},
    $assistant,
    $tool_msgs[]
  ]')

FINAL_RESPONSE=$(curl -s -X POST "https://api.demeterics.com/groq/v1/chat/completions" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<EOF
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
  "messages": $MESSAGES_JSON
}
EOF
)

# Display final response
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Final AI Response${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

FINAL_CONTENT=$(echo "$FINAL_RESPONSE" | jq -r '.choices[0].message.content')
echo -e "${CYAN}$FINAL_CONTENT${NC}"
echo ""

# Display token usage
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Token Usage${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

PROMPT_TOKENS=$(echo "$FINAL_RESPONSE" | jq -r '.usage.prompt_tokens')
COMPLETION_TOKENS=$(echo "$FINAL_RESPONSE" | jq -r '.usage.completion_tokens')
TOTAL_TOKENS=$(echo "$FINAL_RESPONSE" | jq -r '.usage.total_tokens')

echo -e "${YELLOW}Prompt tokens:${NC} $PROMPT_TOKENS"
echo -e "${YELLOW}Completion tokens:${NC} $COMPLETION_TOKENS"
echo -e "${YELLOW}Total tokens:${NC} $TOTAL_TOKENS"
echo ""

# Calculate cost
INPUT_COST=$(echo "scale=6; $PROMPT_TOKENS * 0.11 / 1000000" | bc)
OUTPUT_COST=$(echo "scale=6; $COMPLETION_TOKENS * 0.34 / 1000000" | bc)
TOTAL_COST=$(echo "scale=6; $INPUT_COST + $OUTPUT_COST" | bc)

echo -e "${YELLOW}Cost:${NC} \$$TOTAL_COST"
echo ""

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "✓ AI agent successfully used tools"
echo "✓ Executed $TOOL_CALL_COUNT tool call(s)"
echo "✓ Retrieved real-time information"
echo "✓ Generated informed response"
echo ""
