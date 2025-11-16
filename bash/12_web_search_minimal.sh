#!/bin/sh
###############################################################################
# Example 11 (Minimal): Web Search with groq/compound-mini
#
# What this demonstrates:
#   - Calling Groq's chat API with a web-search-capable model
#   - Minimal cURL-only request (raw JSON output)
#   - Environment variable authentication
#
# What you'll learn:
#   - How to invoke `groq/compound-mini` for questions needing fresh info
#   - How to structure a basic chat completion request
#
# Prerequisites:
#   - DEMETERICS_API_KEY environment variable set
#   - curl installed
#
# Expected output:
#   - Raw JSON with the model's final answer in choices[0].message.content
#   - Optionally, extra fields like message.reasoning or executed tool info
###############################################################################

if [ -z "$DEMETERICS_API_KEY" ]; then
  echo "Error: DEMETERICS_API_KEY not set" 1>&2
  echo "Run: export DEMETERICS_API_KEY=\"dmt_your_api_key_here\"" 1>&2
  exit 1
fi

# Minimal payload: Ask a question that typically requires web search.
# Note: groq/compound-mini has built-in web search; the model will decide
# when to call it. No special tool config is required for this minimal demo.

curl -sS \
  -X POST "https://api.demeterics.com/groq/v1/chat/completions" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<'EOF'
{
  "model": "groq/compound-mini",
  "messages": [
    {"role": "system", "content": "You are a helpful research assistant. Provide concise, cited answers."},
    {"role": "user",   "content": "What were the top 3 AI model releases last week? Include links and brief summaries."}
  ],
  "temperature": 0.3,
  "max_tokens": 500
}
EOF

# Exercises (edit this file and re-run):
# 1) Change the question to something location-specific (e.g., in your country).
# 2) Try a more complex query using AND/OR wording (e.g., "ANNOUNCEMENTS AND (OpenAI OR Meta)").
# 3) Switch to the bigger model by changing model to "groq/compound".
# 4) For advanced control (include/exclude domains, country), see the full version script.

# Quick tip: see exercises for this example (printed to stderr to keep stdout JSON-only)
echo "Exercises: exercises/11_web_search.md" 1>&2
