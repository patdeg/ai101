#!/bin/bash

################################################################################
# EXAMPLE 3: VISION - ANALYZING IMAGES
################################################################################
#
# What this demonstrates:
#   - How to send images to the AI for analysis
#   - Converting images to base64 format for API transmission
#   - Combining text and images in a single request
#   - Using jq to parse and display multi-modal responses
#
# What you'll learn:
#   - Images must be base64-encoded before sending
#   - Content can be an array of multiple types (text + image)
#   - How to use data URLs for embedding images
#   - Vision requests use more tokens (image + text + response)
#
# Prerequisites:
#   - OPENROUTER_API_KEY environment variable must be set
#   - jq command installed (see README.md for installation)
#   - base64 command (pre-installed on Linux/Mac)
#   - test_image.jpg in parent directory
#
# Expected output:
#   - Raw JSON response (pretty-printed)
#   - Detailed image description
#   - Token usage statistics (includes image tokens!)
#   - Cost breakdown
#   - Timing information
#
################################################################################

# Store the API endpoint URL
API_URL="https://openrouter.ai/api/v1/chat/completions"

# STEP 1: Set the path to the image we want to analyze
# Using the shared test image from the root directory
IMAGE_PATH="../test_image.jpg"

# Check if the image exists
if [ ! -f "$IMAGE_PATH" ]; then
  echo "Error: Image not found at $IMAGE_PATH"
  echo "Please make sure test_image.jpg exists in the root directory"
  exit 1
fi

# STEP 2: Convert the image to base64 format
# Why? Because JSON can't contain raw binary image data
# The -w 0 flag means "no line wrapping" - we need one continuous string
echo "Converting image to base64 format..."
IMAGE_BASE64=$(base64 -w 0 "$IMAGE_PATH" 2>/dev/null || base64 "$IMAGE_PATH")

# STEP 3: Build the data URL in the required format
# Format: data:image/jpeg;base64,<encoded_image_data>
# Change "image/jpeg" to "image/png" if using PNG files
IMAGE_URL="data:image/jpeg;base64,$IMAGE_BASE64"

# STEP 4: Send the request with both text and image
# Note: content is now an ARRAY containing multiple items
echo "Sending image to AI for analysis..."
echo ""
RESPONSE=$(curl -s "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -d '{
    "model": "google/gemma-4-31b-it",
    "messages": [
      {
        "role": "user",
        "content": [
          {
            "type": "text",
            "text": "What is in this image? Describe in detail."
          },
          {
            "type": "image_url",
            "image_url": {
              "url": "'"$IMAGE_URL"'"
            }
          }
        ]
      }
    ],
    "temperature": 0.3,
    "max_tokens": 500
  }')

# STEP 5: Display the raw API response (pretty-printed with jq)
echo "========================================"
echo "Raw API Response (pretty-printed):"
echo "========================================"
echo "$RESPONSE" | jq .

# STEP 6: Extract and display the image description
echo ""
echo "========================================"
echo "Image Description:"
echo "========================================"
echo "$RESPONSE" | jq -r '.choices[0].message.content'

# STEP 7: Extract and display token usage statistics
# Note: Vision requests include image tokens in the prompt count!
echo ""
echo "========================================"
echo "Token Usage:"
echo "========================================"
PROMPT_TOKENS=$(echo "$RESPONSE" | jq '.usage.prompt_tokens')
COMPLETION_TOKENS=$(echo "$RESPONSE" | jq '.usage.completion_tokens')
TOTAL_TOKENS=$(echo "$RESPONSE" | jq '.usage.total_tokens')

echo "  Prompt tokens:     $PROMPT_TOKENS  (includes image tokens!)"
echo "  Completion tokens: $COMPLETION_TOKENS"
echo "  Total tokens:      $TOTAL_TOKENS"
echo ""
echo "  💡 The prompt tokens include:"
echo "     - Your text question (~10-20 tokens)"
echo "     - The image itself (500-2000 tokens depending on size)"

# STEP 8: Calculate the cost of this API call
# Pricing for google/gemma-4-31b-it on OpenRouter: $0.09/1M input,
# $0.34/1M output. Not the ":free" tier - that variant was too congested
# upstream to be reliable, so this example uses the small-but-paid version.
VISION_INPUT_COST=$(echo "scale=8; $PROMPT_TOKENS * 0.09 / 1000000" | bc)
VISION_OUTPUT_COST=$(echo "scale=8; $COMPLETION_TOKENS * 0.34 / 1000000" | bc)
VISION_TOTAL_COST=$(echo "scale=8; $VISION_INPUT_COST + $VISION_OUTPUT_COST" | bc)
echo ""
echo "========================================"
echo "Cost Breakdown:"
echo "========================================"
echo "  Input:  \$$VISION_INPUT_COST"
echo "  Output: \$$VISION_OUTPUT_COST"
echo "  Total:  \$$VISION_TOTAL_COST"
echo "  Vision requests cost more than text-only, since the image itself"
echo "  burns 500-2000+ prompt tokens."

################################################################################
# EXPLANATION OF THE API REQUEST
################################################################################
#
# Here's the request body with detailed explanations:
# {
#   "model": "google/gemma-4-31b-it",
#     ↑ A different model than example 1 - this one supports vision!
#     Not all models can analyze images - check a model's "modalities"
#     on https://openrouter.ai/models before assuming it can see
#
#   "messages": [
#     {
#       "role": "user",
#
#       "content": [
#         ↑ IMPORTANT: content is now an ARRAY, not a string!
#         This allows us to combine text + images in one message
#
#         {
#           "type": "text",
#             ↑ First item in the array: text content
#
#           "text": "What is in this image? Describe in detail."
#             ↑ Our question about the image
#             Tell the AI what you want to know about the image
#         },
#
#         {
#           "type": "image_url",
#             ↑ Second item in the array: image content
#
#           "image_url": {
#             "url": "data:image/jpeg;base64,<base64_data>"
#               ↑ The image as a data URL
#               Format: data:<mime-type>;base64,<encoded-image>
#               This is the $IMAGE_URL variable we created earlier
#           }
#         }
#       ]
#     }
#   ],
#
#   "temperature": 0.3,
#     ↑ Lower temperature for more accurate, factual descriptions
#     We want the AI to describe what it actually sees
#     Not to be creative about it
#
#   "max_tokens": 500
#     ↑ More tokens needed for detailed image descriptions
#     Images typically need longer responses than simple Q&A
# }

################################################################################
# KEY CONCEPTS FROM THIS EXAMPLE
################################################################################
#
# Multi-modal content:
#   "content" is an ARRAY instead of a string
#   Can mix text and images in the same message
#
# Content types:
#   "type": "text" = your question/instruction
#   "type": "image_url" = the image to analyze
#
# base64 encoding:
#   Converts binary files (images) to text format
#   -w 0 means "no line wrapping" (all one line)
#   macOS: use just `base64` without -w flag
#
# Data URL format:
#   data:image/jpeg;base64,<data>
#   - data: = this is embedded data, not a web URL
#   - image/jpeg = file type (use image/png for PNG files)
#   - base64 = encoding method
#   - <data> = the actual encoded image
#
# Image limitations:
#   - Max 4 MB after base64 encoding
#   - Max 33 megapixels resolution
#   - Supported: JPEG, PNG, GIF, WebP
#
# Temperature:
#   0.3 = Lower temperature for factual, accurate descriptions
#   We want the AI to describe what it actually sees
#   Not to hallucinate or be overly creative
#
# Token usage for vision:
#   Vision requests use MORE tokens than text-only
#   - Image: 500-2000 tokens (depending on size/resolution)
#   - Text question: 10-20 tokens
#   - Response: 50-500 tokens (depending on detail requested)
#   Total: Usually 600-2500 tokens per vision request
#
# Cost comparison:
#   Text-only request:  ~$0.000002 (20 tokens in, 10 out)
#   Vision request:     ~$0.000200 (1500 tokens in, 100 out)
#   Vision is ~100x more expensive, but still very cheap!
