#!/bin/sh
# Compile a prompt template and use it with AI
# Usage: ./03_prompt_template_minimal.sh "The Renaissance"

TOPIC=${1:-"Climate Change"}
TEMPLATE_FILE="../templates/essay_writer_simple.txt"

# Get current timestamp
NOW=$(date -u +"%Y-%m-%d %H:%M:%S UTC")

# Read and process template (remove /// comments, substitute variables)
PROMPT=$(sed -e '/^\/\/\//d' -e "s/\[\[\.Now\]\]/$NOW/g" "$TEMPLATE_FILE")

# Send to AI
curl -s https://api.groq.com/openai/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d @- <<EOF
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
  "messages": [
    {"role": "system", "content": "$PROMPT"},
    {"role": "user", "content": "Write the essay about $TOPIC"}
  ],
  "max_tokens": 2000,
  "temperature": 0.7
}
EOF

# Parameters explained:
# Template compilation removes /// comments and substitutes variables
# Simpler template without conditionals for bash/sed compatibility
# User provides the specific topic for the essay