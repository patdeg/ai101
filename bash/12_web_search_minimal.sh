#!/bin/sh
###############################################################################
# Example 11 (Minimal): Web Search with google/gemini-3.8-flash:online
#
# What this demonstrates:
#   - Calling OpenRouter's chat API with a web-search-augmented model
#   - Minimal cURL-only request (raw JSON output)
#   - Environment variable authentication
#
# What you'll learn:
#   - How to append ":online" to any model slug for search-augmented answers
#   - How to structure a basic chat completion request
#
# Prerequisites:
#   - OPENROUTER_API_KEY environment variable set
#   - curl installed
#
# Expected output:
#   - Raw JSON with the model's final answer in choices[0].message.content
#   - Optionally, extra fields like message.reasoning or executed tool info
###############################################################################

if [ -z "$OPENROUTER_API_KEY" ]; then
  echo "Error: OPENROUTER_API_KEY not set" 1>&2
  echo "Run: export OPENROUTER_API_KEY=\"sk-or-your_api_key_here\"" 1>&2
  exit 1
fi

# Minimal payload: Ask a question that typically requires web search.
# The ":online" suffix on the model slug turns on OpenRouter's built-in web
# search plugin (default engine: Exa) — the model decides when to use it.
# No special tool config is required for this minimal demo. Each search adds
# roughly $0.005 on top of the underlying model's normal per-token cost.

curl -sS \
  -X POST "https://openrouter.ai/api/v1/chat/completions" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<'EOF'
{
  "model": "google/gemini-3.8-flash:online",
  "messages": [
    {"role": "system", "content": "You are a helpful research assistant. Provide concise, cited answers."},
    {"role": "user",   "content": "What were the top 3 AI model releases last week? Include links and brief summaries."}
  ],
  "temperature": 0.3,
  "max_tokens": 1200
}
EOF

# Exercises (edit this file and re-run):
# 1) Change the question to something location-specific (e.g., in your country).
# 2) Try a more complex query using AND/OR wording (e.g., "ANNOUNCEMENTS AND (OpenAI OR Meta)").
# 3) Try a different model that supports ":online" and compare answer and cost.
# 4) For advanced control (include/exclude domains, country), see the full version script.

# Quick tip: see exercises for this example (printed to stderr to keep stdout JSON-only)
echo "Exercises: exercises/11_web_search.md" 1>&2
