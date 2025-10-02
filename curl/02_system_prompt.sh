#!/bin/bash

################################################################################
# EXAMPLE 2: SYSTEM PROMPT + USER PROMPT
################################################################################
#
# What this demonstrates:
#   - How to use system prompts to control AI behavior
#   - The difference between system and user messages
#   - How to shape AI responses with instructions
#   - Extracting and validating AI responses with jq
#
# What you'll learn:
#   - System prompts act like "job descriptions" for the AI
#   - Message order matters (system first, then user)
#   - How to control response style, format, and personality
#   - How to verify the AI followed your instructions
#
# Prerequisites:
#   - GROQ_API_KEY environment variable must be set
#   - jq command installed (see README.md for installation)
#   - Understanding of Example 1 (basic chat)
#
# Expected output:
#   - Raw JSON response (pretty-printed)
#   - Extracted AI answer (should be exactly 2 sentences)
#   - Token usage statistics
#   - Timing information
#   - Validation that response is exactly 2 sentences
#
################################################################################

# Store the API endpoint URL
API_URL="https://api.groq.com/openai/v1/chat/completions"

# STEP 1: Make the API call with BOTH system and user messages
# The system message comes first to set the AI's behavior
echo "Sending request to AI with system prompt..."
RESPONSE=$(curl -s "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "system",
        "content": "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more."
      },
      {
        "role": "user",
        "content": "Why did the Roman Empire fall?"
      }
    ],
    "temperature": 0.5,
    "max_tokens": 150
  }')

# Here's the request body with detailed explanations:
# {
#   "model": "meta-llama/llama-4-scout-17b-16e-instruct",
#     ↑ Which AI model to use (same as Example 1)
#
#   "messages": [
#     ↑ Array of messages - notice we have TWO messages now!
#
#     {
#       "role": "system",
#         ↑ SYSTEM message defines HOW the AI should respond
#         Sets personality, expertise, constraints, output format
#         Think of this as "programming" the AI's behavior
#
#       "content": "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more."
#         ↑ The system instructions
#         This tells the AI to be: concise, expert, 2 sentences max
#     },
#
#     {
#       "role": "user",
#         ↑ USER message is WHAT question to answer
#         This is the actual query from the user
#
#       "content": "Why did the Roman Empire fall?"
#         ↑ The question we want answered
#         The AI will answer this following the system instructions above
#     }
#   ],
#
#   "temperature": 0.5,
#     ↑ Lower temperature for more consistent, factual responses
#     0.5 = more focused than default (1.0)
#     Good for historical facts where accuracy matters
#
#   "max_tokens": 150
#     ↑ Enough tokens for 2 detailed sentences
#     ~110-120 words
# }

# STEP 2: Display the raw API response (pretty-printed with jq)
echo ""
echo "========================================"
echo "Raw API Response (pretty-printed):"
echo "========================================"
echo "$RESPONSE" | jq .

# STEP 3: Extract and display the AI's answer
echo ""
echo "========================================"
echo "AI's Answer:"
echo "========================================"
ANSWER=$(echo "$RESPONSE" | jq -r '.choices[0].message.content')
echo "$ANSWER"

# STEP 4: Verify the AI followed the system prompt (exactly 2 sentences)
# Count the number of periods (sentences)
SENTENCE_COUNT=$(echo "$ANSWER" | grep -o '\.' | wc -l)
echo ""
echo "Validation: Answer contains $SENTENCE_COUNT sentence(s)"
if [ "$SENTENCE_COUNT" -eq 2 ]; then
  echo "✓ System prompt followed correctly (exactly 2 sentences)"
else
  echo "⚠ System prompt not followed (expected 2 sentences, got $SENTENCE_COUNT)"
fi

# STEP 5: Extract and display token usage statistics
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

# STEP 5b: Calculate the cost of this API call
# Pricing for meta-llama/llama-4-scout-17b-16e-instruct (as of Oct 1, 2025):
#   Input:  $0.11 per 1M tokens
#   Output: $0.34 per 1M tokens
echo ""
echo "========================================"
echo "Cost Breakdown:"
echo "========================================"

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

# STEP 6: Extract and display timing information
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
# KEY CONCEPTS FROM THIS EXAMPLE
################################################################################
#
# System vs User messages:
#   - System message = HOW to respond (personality, rules, constraints)
#   - User message = WHAT to respond to (the actual question)
#   - System message should come FIRST in the messages array
#
# Message order matters:
#   1. System message comes FIRST (sets behavior)
#   2. User message comes SECOND (asks question)
#   3. You can add more user/assistant messages for conversation history
#
# Temperature parameter:
#   - 0.5 is lower than Example 1's 0.7
#   - Lower = more focused, consistent, factual
#   - Higher = more creative, random, diverse
#   - Use low temperature for facts, high for creative writing
#
# Why use system prompts?
#   - Control response length ("be concise" or "explain in detail")
#   - Set expertise level ("explain like I'm 5" or "use technical terms")
#   - Define behavior ("be formal" or "be casual and friendly")
#   - Set output format ("answer in JSON" or "use bullet points")
#   - Enforce constraints ("exactly 2 sentences" or "under 50 words")
#
# Validation with bash:
#   - We can count sentences with: grep -o '\.' | wc -l
#   - We can verify the AI followed instructions
#   - Useful for building reliable AI workflows
