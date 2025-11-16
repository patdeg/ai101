#!/bin/sh
###############################################################################
# Example 12 (Minimal): Code Execution with openai/gpt-oss-20b
#
# What this demonstrates:
#   - Using a code-execution-capable model to run Python
#   - Minimal cURL request (raw JSON output)
#
# What you'll learn:
#   - How to send a task that is best solved by executing code
#   - How to ask the model to check for exceptions and mitigate them
#
# Prerequisites:
#   - DEMETERICS_API_KEY environment variable set
#   - curl installed
#
# Expected output:
#   - Raw JSON containing the final answer and optional reasoning/tool info
###############################################################################

if [ -z "$DEMETERICS_API_KEY" ]; then
  echo "Error: DEMETERICS_API_KEY not set" 1>&2
  echo "Run: export DEMETERICS_API_KEY=\"dmt_your_api_key_here\"" 1>&2
  exit 1
fi

# Prompt idea: Ask the model to execute Python that would raise, then fix it.
curl -sS \
  -X POST "https://api.demeterics.com/groq/v1/chat/completions" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<'EOF'
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {"role": "system", "content": "You can execute Python to verify results. If code raises an exception, show it and provide a corrected version."},
    {"role": "user",   "content": "1) Compute the 2000th prime number using Python. 2) Execute Python: print(1/0) to show the exception, then show how to handle it gracefully."}
  ],
  "tools": [
    {"type": "code_interpreter"}
  ],
  "tool_choice": "required",
  "temperature": 0.2,
  "max_tokens": 700
}
EOF

# Exercises:
# 1) Change model to "openai/gpt-oss-120b" (if available) and compare results.
# 2) Ask the model to run code that might fail (e.g., parsing invalid JSON) and fix it.
# 3) Ask for the code snippet it actually executed and its outputs.

# Print exercises path to stderr to keep stdout JSON-only
echo "Exercises: exercises/12_code_execution.md" 1>&2
