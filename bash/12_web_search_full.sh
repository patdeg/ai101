#!/bin/bash
################################################################################
# Example 11 (Full): Web Search with groq/compound-mini + Tavily
#
# What this demonstrates:
#   - Using Groq's web-search-enabled model `groq/compound-mini`
#   - Pretty-printing and extracting useful fields with jq
#   - Inspecting optional reasoning and executed tool info
#
# What you'll learn:
#   - How to structure a thorough chat completion request
#   - How to view `message.reasoning` and executed tool results if present
#   - Where to extend with domain filters and country hints
#
# Prerequisites:
#   - GROQ_API_KEY environment variable set
#   - curl and jq installed
#
# Expected output:
#   - Pretty JSON
#   - Final answer content
#   - Optional reasoning/executed tools if the model used web search
################################################################################

set -euo pipefail

if ! command -v curl >/dev/null; then
  echo "Error: curl is required" 1>&2; exit 1
fi
if ! command -v jq >/dev/null; then
  echo "Error: jq is required (install with: brew install jq or apt-get install jq)" 1>&2; exit 1
fi
if [ -z "${GROQ_API_KEY:-}" ]; then
  echo "Error: GROQ_API_KEY not set" 1>&2
  echo "Run: export GROQ_API_KEY=\"gsk_your_api_key_here\"" 1>&2
  exit 1
fi

API_URL="https://api.groq.com/openai/v1/chat/completions"

echo "========================================"
echo "Web Search with groq/compound-mini"
echo "========================================"
echo "Question: Top AI model releases last week with links"
echo

# Note:
# - The groq/compound-mini model can use integrated web search.
# - In many cases, you do not need to pass tool parameters; the model will
#   decide when/how to search. When it searches, you may see extra fields such as
#   message.reasoning and message.executed_tools.
# - Some deployments support optional search controls like include/exclude domains
#   or country hints. If supported in your account, consult the docs and add them
#   to the payload (see the EXERCISES section below for guidance).

PAYLOAD=$(cat <<'JSON'
{
  "model": "groq/compound-mini",
  "messages": [
    {"role": "system", "content": "You are a helpful research assistant. Provide concise answers with source links. If search is needed, use it."},
    {"role": "user",   "content": "What were the top 3 AI model releases last week? Include links and 1-sentence summaries."}
  ],
  "temperature": 0.3,
  "max_tokens": 600
}
JSON
)

RESPONSE=$(curl -sS \
  -X POST "$API_URL" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -H "Content-Type: application/json" \
  -d "$PAYLOAD")

echo "Raw (pretty):"
if printf '%s' "$RESPONSE" | jq '.' >/dev/null 2>&1; then
  printf '%s' "$RESPONSE" | jq '.'
else
  echo "(Response was not valid JSON. Raw below:)"
  echo "----------------------------------------"
  printf '%s\n' "$RESPONSE"
fi
echo

# Safely extract fields only if JSON is valid
if printf '%s' "$RESPONSE" | jq '.' >/dev/null 2>&1; then
  echo "========================================"
  echo "Final Answer"
  echo "========================================"
  printf '%s' "$RESPONSE" | jq -r '.choices[0].message.content // "(no content)"'
  echo

  echo "========================================"
  echo "Optional Reasoning"
  echo "========================================"
  if printf '%s' "$RESPONSE" | jq -e '.choices[0].message.reasoning' >/dev/null 2>&1; then
    printf '%s' "$RESPONSE" | jq -r '.choices[0].message.reasoning'
  else
    echo "(No reasoning provided by model)"
  fi
  echo

  echo "========================================"
  echo "Executed Tools (if any)"
  echo "========================================"
  if printf '%s' "$RESPONSE" | jq -e '.choices[0].message.executed_tools' >/dev/null 2>&1; then
    printf '%s' "$RESPONSE" | jq -r '.choices[0].message.executed_tools'
  else
    echo "(No executed tool info present)"
  fi
  echo
fi

################################################################################
# EXERCISES (Try these by editing payload and re-running)
################################################################################
# 1) Model size:
#    Change "model" to "groq/compound" for potentially stronger results.
#
# 2) Query logic:
#    Ask a boolean-style query, e.g.: "(OpenAI OR Meta) AND release notes last 7 days".
#
# 3) Domain filters (when supported):
#    Some deployments allow passing search config (e.g., include/exclude domains, country).
#    Check the Groq Web Search docs for the latest schema. A common pattern is providing
#    a configuration object for the integrated search tool. For example, you may find a
#    vendor-specific field such as:
#
#    "web_search": {
#      "provider": "tavily",
#      "include_domains": ["techcrunch.com", "arxiv.org"],
#      "exclude_domains": ["reddit.com"],
#      "country": "us"
#    }
#
#    IMPORTANT: Only include fields that your account/docs confirm are supported.
#    If unsupported, the request may fail.

# Learn more exercises for this example:
echo "Exercises: exercises/11_web_search.md"
