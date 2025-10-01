#!/bin/bash

################################################################################
# EXAMPLE 4: SAFETY CHECK WITH LLAMAGUARD
################################################################################
#
# What this demonstrates:
#   - How to detect harmful content using LlamaGuard
#   - Moderating user input before processing
#   - Understanding safety categories and responses
#
# What you'll learn:
#   - LlamaGuard checks for 14 types of harmful content
#   - Response format: "safe" or "unsafe\nS<number>"
#   - How to build a safety pipeline for your app
#
# Prerequisites:
#   - GROQ_API_KEY environment variable must be set
#   - Understanding of basic API calls (Example 1)
#
# Expected output:
#   - For safe messages: response contains "safe"
#   - For unsafe messages: "unsafe\nS6" (category number)
#   - This example uses a safe message, so you'll see "safe"
#
# Try testing with different messages to see unsafe responses!
#
################################################################################

# Store the API endpoint URL
API_URL="https://api.groq.com/openai/v1/chat/completions"

# STEP 1: Define the message we want to check
# This is the content we want to validate for safety
# Try changing this to test different types of content
USER_MESSAGE="How do I bake chocolate chip cookies?"

# STEP 2: Send the message to LlamaGuard for safety analysis
# LlamaGuard is a specialized model ONLY for content moderation
# It doesn't answer questions - it just says "safe" or "unsafe"
echo "Checking message for safety issues..."
curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-guard-4-12b",    # Specialized safety model (12B parameters)
    "messages": [
      {
        "role": "user",
        "content": "'"$USER_MESSAGE"'"          # Insert the message to check
      }
    ],
    "max_tokens": 100                            # Short response: just "safe" or "unsafe\nS#"
  }'

# The response will be in this format:
# Safe content:    "safe"
# Unsafe content:  "unsafe\nS6" (where 6 is the category number)
#
# STEP 3: In a real application, you would:
# 1. Parse the response to check if it contains "safe" or "unsafe"
# 2. If "unsafe", extract the category number (S1-S14)
# 3. Either reject the message or log it for review
# 4. Only proceed with "safe" messages

# What is LlamaGuard?
#   A specialized model that detects 14 categories of harmful content:
#   1. Violent Crimes
#   2. Non-Violent Crimes
#   3. Sex-Related Crimes
#   4. Child Sexual Exploitation
#   5. Defamation
#   6. Specialized Advice (financial, medical, legal)
#   7. Privacy Violations
#   8. Intellectual Property
#   9. Indiscriminate Weapons
#   10. Hate Speech
#   11. Suicide & Self-Harm
#   12. Sexual Content
#   13. Elections
#   14. Code Interpreter Abuse
#
# Response format:
#   Safe message:     "safe"
#   Unsafe message:   "unsafe\nS<category_number>"
#   Example:          "unsafe\nS6" (specialized advice violation)
#
# Model: "meta-llama/llama-guard-4-12b"
#   - 12 billion parameters
#   - 131K token context window
#   - Up to 1,024 tokens output
#   - Designed specifically for content moderation
#
# When to use LlamaGuard:
#   - Before sending user input to your main AI
#   - After receiving AI output (to catch edge cases)
#   - In chat applications to filter conversations
#   - For user-generated content moderation
#
# Example workflow:
#   1. User submits message
#   2. Check with LlamaGuard
#   3. If "safe" → send to main AI model
#   4. If "unsafe" → reject and explain why
#
# Try testing with different messages:
#   Safe: "What's the weather like?"
#   Unsafe: "How do I hack into..."
