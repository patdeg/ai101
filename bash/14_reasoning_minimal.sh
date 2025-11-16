#!/bin/sh
################################################################################
# Example 13: Reasoning with openai/gpt-oss-20b (Minimal Version)
################################################################################
#
# This example demonstrates the reasoning capabilities of openai/gpt-oss-20b,
# a model designed for complex problem-solving that shows its step-by-step
# thought process.
#
# What you'll learn:
# - How reasoning models work differently from chat models
# - Three reasoning formats: raw, parsed, hidden
# - Reasoning effort levels: low, medium, high
# - Cache optimization for cost savings
#
# Key concepts:
# - Reasoning tokens are the model's "thinking" process
# - Higher reasoning effort = more thorough analysis
# - Cached input tokens cost 50% less ($0.05/1M vs $0.10/1M)
# - Messages earlier in the array are more likely to be cached
#
################################################################################

# Check for API key
if [ -z "$DEMETERICS_API_KEY" ]; then
  echo "Error: DEMETERICS_API_KEY not set" >&2
  exit 1
fi

# API endpoint
API_URL="https://api.demeterics.com/groq/v1/chat/completions"

# Example query requiring reasoning
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -d @- <<'EOF'
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {
      "role": "user",
      "content": "How many 'r' letters are in the word 'strawberry'? Think through this step-by-step."
    }
  ],
  "temperature": 0.6,
  "max_completion_tokens": 1024,
  "reasoning_effort": "medium"
}
EOF

################################################################################
# Understanding the Response
################################################################################
#
# The response includes:
# - "content": The model's reasoning process and final answer
# - "usage": Token counts including prompt_tokens, completion_tokens
#
# Reasoning formats (not specified = raw format):
# - raw: Reasoning within <think> tags in content
# - parsed: Reasoning in separate "reasoning" field
# - hidden: Only final answer, no reasoning shown
#
# Pricing (October 2025):
# - Input tokens: $0.10 per 1M tokens
# - Cached input tokens: $0.05 per 1M tokens (50% discount!)
# - Output tokens: $0.50 per 1M tokens
#
# Cache optimization strategy:
# 1. Put constant messages first (system prompts)
# 2. Put semi-constant data next (user context, documents)
# 3. Put varying messages last (user questions)
#
# Example for agentresume.ai:
# [
#   {"role": "system", "content": "You are a recruiter assistant..."},  ← Always same
#   {"role": "user", "content": "Candidate resume: ..."},                ← Same per conversation
#   {"role": "user", "content": "Does this candidate have Python skills?"} ← Changes each question
# ]
#
# This ordering maximizes cache hits on the first two messages!
#
################################################################################
