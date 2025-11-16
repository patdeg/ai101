#!/bin/bash

################################################################################
# EXAMPLE 1: BASIC CHAT
################################################################################
#
# What this demonstrates:
#   - How to make your first API call to an AI model
#   - Sending a simple question and receiving a response
#   - Basic structure of the chat completions API
#   - Using jq to parse and extract data from JSON responses
#
# What you'll learn:
#   - How to structure a chat API request
#   - What headers are required for authentication
#   - How to control response behavior with parameters
#   - How to extract specific fields from JSON with jq
#
# Prerequisites:
#   - DEMETERICS_API_KEY environment variable must be set
#   - curl command installed (usually pre-installed on Linux/Mac)
#   - jq command installed (see README.md for installation)
#
# Expected output:
#   - Raw JSON response (pretty-printed)
#   - Extracted AI answer
#   - Token usage statistics
#   - Timing information
#
################################################################################

# Store the API endpoint URL in a variable for easy reference
# This is Groq's chat completions endpoint (OpenAI-compatible)
API_URL="https://api.demeterics.com/groq/v1/chat/completions"

# STEP 1: Make the API call and capture the response in a variable
# We're using curl to send an HTTP POST request
# The response is stored in RESPONSE so we can process it multiple times
echo "Sending request to AI..."
RESPONSE=$(curl -s "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "user",
        "content": "What is the capital of Switzerland?"
      }
    ],
    "temperature": 0.7,
    "max_tokens": 100
  }')

# STEP 2: Display the raw API response (pretty-printed with jq)
# The jq . command formats JSON nicely with indentation and colors
echo ""
echo "========================================"
echo "Raw API Response (pretty-printed):"
echo "========================================"
echo "$RESPONSE" | jq .

# STEP 3: Extract and display the AI's answer
# jq -r removes the quotes around the string for clean output
echo ""
echo "========================================"
echo "AI's Answer:"
echo "========================================"
echo "$RESPONSE" | jq -r '.choices[0].message.content'

# STEP 4: Extract and display token usage statistics
# Tokens are the "units" of text - roughly 0.75 words per token
# You're billed separately for input (prompt) and output (completion) tokens
echo ""
echo "========================================"
echo "Token Usage:"
echo "========================================"
PROMPT_TOKENS=$(echo "$RESPONSE" | jq '.usage.prompt_tokens')
COMPLETION_TOKENS=$(echo "$RESPONSE" | jq '.usage.completion_tokens')
TOTAL_TOKENS=$(echo "$RESPONSE" | jq '.usage.total_tokens')

echo "  Prompt tokens:     $PROMPT_TOKENS"
echo "  Completion tokens: $COMPLETION_TOKENS"
echo "  Total tokens:      $TOTAL_TOKENS"

# STEP 4b: Calculate the cost of this API call
# Pricing for meta-llama/llama-4-scout-17b-16e-instruct (as of Oct 1, 2025):
#   Input:  $0.11 per 1M tokens
#   Output: $0.34 per 1M tokens
echo ""
echo "========================================"
echo "Cost Breakdown:"
echo "========================================"

# Calculate cost in dollars (using bc for floating point math)
# If bc is not available, we'll use awk instead
if command -v bc &> /dev/null; then
  INPUT_COST=$(echo "scale=10; $PROMPT_TOKENS * 0.11 / 1000000" | bc)
  OUTPUT_COST=$(echo "scale=10; $COMPLETION_TOKENS * 0.34 / 1000000" | bc)
  TOTAL_COST=$(echo "scale=10; $INPUT_COST + $OUTPUT_COST" | bc)
else
  INPUT_COST=$(awk "BEGIN {printf \"%.10f\", $PROMPT_TOKENS * 0.11 / 1000000}")
  OUTPUT_COST=$(awk "BEGIN {printf \"%.10f\", $COMPLETION_TOKENS * 0.34 / 1000000}")
  TOTAL_COST=$(awk "BEGIN {printf \"%.10f\", $INPUT_COST + $OUTPUT_COST}")
fi

echo "  Input cost:  \$$INPUT_COST  ($PROMPT_TOKENS tokens × \$0.11/1M)"
echo "  Output cost: \$$OUTPUT_COST  ($COMPLETION_TOKENS tokens × \$0.34/1M)"
echo "  Total cost:  \$$TOTAL_COST"
echo ""
echo "  💡 This request cost less than a fraction of a cent!"

# STEP 5: Extract and display timing information
# These show how long each stage took (in seconds)
echo ""
echo "========================================"
echo "Timing (seconds):"
echo "========================================"
echo "  Queue time:      $(echo "$RESPONSE" | jq '.usage.queue_time')"
echo "  Prompt time:     $(echo "$RESPONSE" | jq '.usage.prompt_time')"
echo "  Completion time: $(echo "$RESPONSE" | jq '.usage.completion_time')"
echo "  Total time:      $(echo "$RESPONSE" | jq '.usage.total_time')"
echo ""

################################################################################
# EXPLANATION OF THE API REQUEST
################################################################################
#
# What each part of the curl command means:
#
# curl -s
#   -s = silent mode (no progress bar)
#
# -H "Content-Type: application/json"
#   Tells the server we're sending JSON data
#
# -H "Authorization: Bearer $DEMETERICS_API_KEY"
#   Proves we have permission to use the API
#   $DEMETERICS_API_KEY is read from your environment variable
#
# -d '{...}'
#   The data we're sending (the -d flag means "data")
#
# Here's the request body with detailed explanations:
# {
#   "model": "meta-llama/llama-4-scout-17b-16e-instruct",
#     ↑ Which AI model to use
#     This is a 17B parameter model good for chat + vision
#     Context window: 131,072 tokens
#     Max output: 8,192 tokens
#
#   "messages": [
#     ↑ Array of conversation messages
#     Can include system, user, and assistant messages
#
#     {
#       "role": "user",
#         ↑ Who is sending this message
#         Options: "system", "user", "assistant"
#         "user" = you (the person asking)
#
#       "content": "What is the capital of Switzerland?"
#         ↑ The actual message text
#         This is your question or prompt
#     }
#   ],
#
#   "temperature": 0.7,
#     ↑ Controls randomness/creativity
#     Range: 0.0 to 2.0
#     0.0 = deterministic (same answer every time)
#     0.7 = balanced (good default)
#     2.0 = very creative/random
#
#   "max_tokens": 100
#     ↑ Maximum length of response
#     1 token ≈ 0.75 words (roughly 4 characters)
#     100 tokens ≈ 75 words
# }

################################################################################
# UNDERSTANDING JQ COMMANDS
################################################################################
#
# jq is a powerful JSON processor. Here's what we used above:
#
# jq .
#   Pretty-prints the entire JSON response with colors and indentation
#
# jq -r '.choices[0].message.content'
#   -r = "raw" output (removes quotes)
#   .choices[0] = first element of the "choices" array
#   .message.content = the actual text response from the AI
#
# jq '.usage.prompt_tokens'
#   Navigate to nested fields with dots
#   .usage = the usage object
#   .prompt_tokens = tokens in your request
#
# Other useful jq commands you can try:
#
#   Get the full message object:
#     echo "$RESPONSE" | jq '.choices[0].message'
#
#   Get both answer and token count in one object:
#     echo "$RESPONSE" | jq '{answer: .choices[0].message.content, tokens: .usage.total_tokens}'
#
#   Get the model name:
#     echo "$RESPONSE" | jq -r '.model'
#
#   Check the finish reason (should be "stop" for normal completion):
#     echo "$RESPONSE" | jq -r '.choices[0].finish_reason'
