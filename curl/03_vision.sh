#!/bin/bash

# Example 3: Vision - Analyzing Images
# The AI can "see" and analyze images by encoding them as base64.

API_URL="https://api.groq.com/openai/v1/chat/completions"

# Path to your image file (change this to your actual image)
IMAGE_PATH="./test_image.jpg"

# Convert image to base64 (required for sending to API)
# base64 encoding turns binary image data into text that can be sent in JSON
IMAGE_BASE64=$(base64 -w 0 "$IMAGE_PATH")

# Build the data URL
# Format: data:image/jpeg;base64,<encoded_image_data>
IMAGE_URL="data:image/jpeg;base64,$IMAGE_BASE64"

curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
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
  }'

# New concepts in this example:
#
# base64 encoding:
#   Converts binary files (images) to text format
#   -w 0 means "no line wrapping" (all one line)
#
# "content": [...]
#   Instead of a simple string, content is now an ARRAY
#   This lets us combine text + images
#
# Content types:
#   "type": "text" = your question
#   "type": "image_url" = the image to analyze
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
# "temperature": 0.3
#   Lower = more accurate, factual descriptions
#   Higher = more creative interpretations
#
# Creating a test image:
#   If you don't have an image, create one:
#   curl -o test_image.jpg https://picsum.photos/400/300
