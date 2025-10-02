#!/bin/bash
################################################################################
# Example 12 (Full): Code Execution with openai/gpt-oss-20b
#
# What this demonstrates:
#   - Using a model with Python code execution to solve tasks
#   - Pretty-printing JSON, extracting final answer, reasoning, and executed tools
#
# What you'll learn:
#   - How to craft prompts that the model solves via code execution
#   - How to inspect code that was run and its outputs (when provided)
#
# Prerequisites:
#   - GROQ_API_KEY set
#   - curl + jq installed
#
# Expected output:
#   - Final answer content
#   - message.reasoning showing tool calls (Python) and outputs
#   - executed_tools with raw code and metadata (if provided)
################################################################################

set -euo pipefail

if ! command -v curl >/dev/null; then echo "curl required" 1>&2; exit 1; fi
if ! command -v jq >/dev/null; then echo "jq required" 1>&2; exit 1; fi
if [ -z "${GROQ_API_KEY:-}" ]; then
  echo "Error: GROQ_API_KEY not set" 1>&2
  echo "Run: export GROQ_API_KEY=\"gsk_your_api_key_here\"" 1>&2
  exit 1
fi

API_URL="https://api.groq.com/openai/v1/chat/completions"

PAYLOAD=$(cat <<'JSON'
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {"role": "system", "content": "You can execute Python to verify results. If code raises an exception, show it and provide a corrected version."},
    {"role": "user",   "content": "1) Compute the 2000th prime number using Python. 2) Execute Python: print(1/0) to demonstrate the exception, then show how to catch it without crashing."}
  ],
  "tools": [
    {"type": "code_interpreter"}
  ],
  "tool_choice": "required",
  "temperature": 0.2,
  "max_tokens": 900
}
JSON
)

echo "========================================"
echo "Code Execution: openai/gpt-oss-20b"
echo "========================================"
echo

RESPONSE=$(curl -sS -X POST "$API_URL" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -H "Content-Type: application/json" \
  -d "$PAYLOAD")

echo "Raw (pretty):"
echo "$RESPONSE" | jq '.'
echo

echo "========================================"
echo "Final Answer"
echo "========================================"
echo "$RESPONSE" | jq -r '.choices[0].message.content'
echo

echo "========================================"
echo "Reasoning & Tool Calls (if any)"
echo "========================================"
if echo "$RESPONSE" | jq -e '.choices[0].message.reasoning' >/dev/null 2>&1; then
  echo "$RESPONSE" | jq -r '.choices[0].message.reasoning'
else
  echo "(No reasoning provided by model)"
fi
echo

echo "========================================"
echo "Executed Tools (if any)"
echo "========================================"
if echo "$RESPONSE" | jq -e '.choices[0].message.executed_tools' >/dev/null 2>&1; then
  echo "$RESPONSE" | jq -r '.choices[0].message.executed_tools'
else
  echo "(No executed tool info present)"
fi
echo

# Exercises:
# 1) Change model to "openai/gpt-oss-120b" (if available) and compare outputs.
# 2) Ask the model to run code that may raise exceptions (e.g., invalid JSON) and show try/except mitigation.
# 3) Ask for the exact code it executed and the captured stdout/stderr.

echo "Exercises: exercises/12_code_execution.md"
