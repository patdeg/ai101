#!/bin/bash
################################################################################
# Example 11 (Full): Web Search with OpenRouter's ":online" web plugin
#
# What this demonstrates:
#   - Using OpenRouter's built-in web-search plugin (the ":online" suffix)
#   - Pretty-printing and extracting useful fields with jq
#   - Inspecting optional reasoning and citation info, if present
#
# What you'll learn:
#   - How to structure a thorough chat completion request
#   - How to view `message.reasoning` and any tool/citation results present
#   - Where to extend with domain filters and result-count controls
#
# Prerequisites:
#   - OPENROUTER_API_KEY environment variable set
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
if [ -z "${OPENROUTER_API_KEY:-}" ]; then
  echo "Error: OPENROUTER_API_KEY not set" 1>&2
  echo "Run: export OPENROUTER_API_KEY=\"sk-or-your_api_key_here\"" 1>&2
  exit 1
fi

API_URL="https://openrouter.ai/api/v1/chat/completions"

echo "========================================"
echo "Web Search with openai/gpt-oss-20b:online"
echo "========================================"
echo "Question: Top AI model releases last week with links"
echo

# Note:
# - Appending ":online" to any model slug turns on OpenRouter's built-in web
#   search plugin (default engine: Exa) for that request.
# - You do not need to pass tool parameters; the model decides when to
#   search. When it does, you may see extra fields such as message.reasoning
#   and message annotations/citations, depending on the model.
# - Each search costs roughly $0.005 (covers up to 10 results) on top of the
#   underlying model's normal per-token price.
# - Optional search controls (max_results, search_prompt, engine) can be
#   passed via a "plugins" array — see OpenRouter's web search docs (see the
#   EXERCISES section below for guidance).

PAYLOAD=$(cat <<'JSON'
{
  "model": "openai/gpt-oss-20b:online",
  "messages": [
    {"role": "system", "content": "You are a helpful research assistant. Provide concise answers with source links. If search is needed, use it."},
    {"role": "user",   "content": "What were the top 3 AI model releases last week? Include links and 1-sentence summaries."}
  ],
  "temperature": 0.3,
  "max_tokens": 1200
}
JSON
)

RESPONSE=$(curl -sS \
  -X POST "$API_URL" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
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
#    Change "model" to e.g. "openai/gpt-oss-120b:online" for potentially
#    stronger results (no longer a ":free" model, so check pricing first).
#
# 2) Query logic:
#    Ask a boolean-style query, e.g.: "(OpenAI OR Meta) AND release notes last 7 days".
#
# 3) Search controls:
#    OpenRouter lets you configure the web plugin explicitly instead of just
#    using the ":online" shorthand — swap "model" back to the plain slug and
#    add a "plugins" array instead:
#
#    "plugins": [
#      {
#        "id": "web",
#        "max_results": 5,
#        "search_prompt": "Focus on official announcements, not rumors."
#      }
#    ]
#
#    See https://openrouter.ai/docs for the current plugin schema.

# Learn more exercises for this example:
echo "Exercises: exercises/11_web_search.md"
