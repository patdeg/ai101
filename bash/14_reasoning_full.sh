#!/bin/bash
################################################################################
# Example 13: Reasoning with openai/gpt-oss-20b (Full Version)
################################################################################
#
# This example demonstrates the reasoning capabilities of openai/gpt-oss-20b,
# a model designed for complex problem-solving that shows its step-by-step
# thought process.
#
# What you'll learn:
# - How reasoning models work differently from chat models
# - Three reasoning formats: raw, parsed, hidden
# - Three reasoning effort levels: low, medium, high
# - Cache optimization strategies for cost savings
# - Token usage and cost tracking
#
# Prerequisites:
# - jq (for JSON parsing)
# - bc (for cost calculations)
# - GROQ_API_KEY environment variable
#
################################################################################

# Color codes for output
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

# Check for API key
if [ -z "$GROQ_API_KEY" ]; then
  echo -e "${RED}Error: GROQ_API_KEY not set${NC}"
  exit 1
fi

API_URL="https://api.groq.com/openai/v1/chat/completions"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Reasoning with openai/gpt-oss-20b${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Example query requiring reasoning
QUERY="How many 'r' letters are in the word 'strawberry'? Think through this step-by-step."

echo -e "${YELLOW}Query:${NC} $QUERY"
echo ""

################################################################################
# Demonstration 1: Raw Format (default)
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 1: Raw Format (reasoning in <think> tags)${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

RESPONSE_RAW=$(curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d @- <<EOF
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {
      "role": "user",
      "content": "$QUERY"
    }
  ],
  "temperature": 0.6,
  "max_completion_tokens": 1024,
  "reasoning_effort": "medium"
}
EOF
)

# Extract and display content
CONTENT=$(echo "$RESPONSE_RAW" | jq -r '.choices[0].message.content')
echo -e "${GREEN}Response (raw format):${NC}"
echo "$CONTENT"
echo ""

# Display token usage
PROMPT_TOKENS=$(echo "$RESPONSE_RAW" | jq -r '.usage.prompt_tokens')
COMPLETION_TOKENS=$(echo "$RESPONSE_RAW" | jq -r '.usage.completion_tokens')
TOTAL_TOKENS=$(echo "$RESPONSE_RAW" | jq -r '.usage.total_tokens')

echo -e "${YELLOW}Token Usage (Demo 1):${NC}"
echo "  Prompt tokens: $PROMPT_TOKENS"
echo "  Completion tokens: $COMPLETION_TOKENS"
echo "  Total tokens: $TOTAL_TOKENS"
echo ""

################################################################################
# Demonstration 2: Parsed Format
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 2: Parsed Format (separate reasoning field)${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

RESPONSE_PARSED=$(curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d @- <<EOF
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {
      "role": "user",
      "content": "$QUERY"
    }
  ],
  "temperature": 0.6,
  "max_completion_tokens": 1024,
  "reasoning_effort": "medium",
  "reasoning_format": "parsed"
}
EOF
)

# Extract reasoning and content separately
REASONING=$(echo "$RESPONSE_PARSED" | jq -r '.choices[0].message.reasoning // "No reasoning field"')
CONTENT_PARSED=$(echo "$RESPONSE_PARSED" | jq -r '.choices[0].message.content')

echo -e "${GREEN}Reasoning process:${NC}"
echo "$REASONING"
echo ""
echo -e "${GREEN}Final answer:${NC}"
echo "$CONTENT_PARSED"
echo ""

################################################################################
# Demonstration 3: Hidden Format
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 3: Hidden Format (only final answer)${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

RESPONSE_HIDDEN=$(curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d @- <<EOF
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {
      "role": "user",
      "content": "$QUERY"
    }
  ],
  "temperature": 0.6,
  "max_completion_tokens": 1024,
  "reasoning_effort": "medium",
  "reasoning_format": "hidden"
}
EOF
)

CONTENT_HIDDEN=$(echo "$RESPONSE_HIDDEN" | jq -r '.choices[0].message.content')
echo -e "${GREEN}Response (hidden format):${NC}"
echo "$CONTENT_HIDDEN"
echo ""

################################################################################
# Cost Calculation
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Cost Analysis${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Pricing for openai/gpt-oss-20b (October 2025)
INPUT_COST_PER_1M=0.10
CACHED_INPUT_COST_PER_1M=0.05
OUTPUT_COST_PER_1M=0.50

# Calculate costs (assuming no cached tokens for this demo)
INPUT_COST=$(echo "scale=6; $PROMPT_TOKENS * $INPUT_COST_PER_1M / 1000000" | bc)
OUTPUT_COST=$(echo "scale=6; $COMPLETION_TOKENS * $OUTPUT_COST_PER_1M / 1000000" | bc)
TOTAL_COST=$(echo "scale=6; $INPUT_COST + $OUTPUT_COST" | bc)

echo -e "${YELLOW}Pricing (openai/gpt-oss-20b):${NC}"
echo "  Input tokens: \$0.10 per 1M"
echo "  Cached input tokens: \$0.05 per 1M (50% discount!)"
echo "  Output tokens: \$0.50 per 1M"
echo ""

echo -e "${YELLOW}This request:${NC}"
echo "  Input cost: \$$INPUT_COST ($PROMPT_TOKENS tokens)"
echo "  Output cost: \$$OUTPUT_COST ($COMPLETION_TOKENS tokens)"
echo "  Total cost: \$$TOTAL_COST"
echo ""

################################################################################
# Cache Optimization Explanation
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Cache Optimization Strategy${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

echo -e "${GREEN}What is cached input?${NC}"
echo "Groq caches parts of your prompt to speed up responses and reduce costs."
echo "Cached tokens are 50% cheaper (\$0.05/1M vs \$0.10/1M for this model)."
echo ""

echo -e "${GREEN}How to maximize cache hits:${NC}"
echo "Order messages from most constant to most variable:"
echo ""
echo "  1. ${CYAN}System prompts${NC} (always the same across requests)"
echo "  2. ${CYAN}Context/documents${NC} (same during a conversation)"
echo "  3. ${CYAN}User questions${NC} (changes with each request)"
echo ""

echo -e "${GREEN}Example: agentresume.ai recruiter chatbot${NC}"
echo ""
echo "Optimal message order:"
cat <<'EXAMPLE'
[
  {
    "role": "system",
    "content": "You are a recruiter assistant. Analyze resumes..."
  },                                                    ← Always cached
  {
    "role": "user",
    "content": "Candidate Resume:\nJohn Doe\nSkills: Python, Go..."
  },                                                    ← Cached per conversation
  {
    "role": "user",
    "content": "Does this candidate have Python experience?"
  }                                                     ← Not cached (changes each Q)
]
EXAMPLE
echo ""

echo -e "${GREEN}Cache benefit calculation:${NC}"
echo "If 1000 tokens are always cached (system + resume):"
echo "  Normal cost: 1000 × \$0.10 / 1M = \$0.0001"
echo "  Cached cost: 1000 × \$0.05 / 1M = \$0.00005"
echo "  Savings: 50% per request!"
echo ""

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Reasoning Effort Comparison${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

echo "Reasoning effort levels:"
echo "  ${CYAN}low${NC}    - Quick analysis, fewer reasoning tokens"
echo "  ${CYAN}medium${NC} - Balanced reasoning (default)"
echo "  ${CYAN}high${NC}   - Deep analysis, more reasoning tokens"
echo ""
echo "Use 'low' for simple questions, 'high' for complex reasoning tasks."
echo ""

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Summary${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "✓ Demonstrated three reasoning formats"
echo "✓ Showed token usage and costs"
echo "✓ Explained cache optimization strategy"
echo "✓ Model excels at step-by-step logical reasoning"
echo ""
