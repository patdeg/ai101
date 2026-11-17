#!/bin/sh
# Analyze an image with AI vision
# Usage: ./03_vision_minimal.sh

# Base64 encode the test image from root directory
IMAGE_BASE64=$(base64 -w 0 ../test_image.jpg 2>/dev/null || base64 ../test_image.jpg)

curl -s https://openrouter.ai/api/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -d @- <<EOF
{
  "model": "google/gemini-3.8-flash",
  "messages": [{
    "role": "user",
    "content": [
      {"type": "text", "text": "What's in this image?"},
      {"type": "image_url", "image_url": {"url": "data:image/jpeg;base64,$IMAGE_BASE64"}}
    ]
  }],
  "max_tokens": 300
}
EOF

# Parameters explained:
# model - google/gemini-3.8-flash, a cheap vision-capable model on OpenRouter
# content array - Mix of text and image_url types
# image_url - Base64-encoded image data URI
