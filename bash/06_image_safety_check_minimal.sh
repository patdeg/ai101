#!/bin/sh
# Check if image content is safe (LlamaGuard Vision)
# Usage: ./05_image_safety_check_minimal.sh

# Base64 encode an image
IMAGE_BASE64=$(base64 -w 0 image.jpg 2>/dev/null || base64 -i image.jpg)

curl -s https://api.demeterics.com/groq/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -d @- <<EOF
{
  "model": "meta-llama/llama-guard-4-12b",
  "messages": [{
    "role": "user",
    "content": [
      {"type": "text", "text": "Check this image for safety"},
      {"type": "image_url", "image_url": {"url": "data:image/jpeg;base64,$IMAGE_BASE64"}}
    ]
  }],
  "max_tokens": 100
}
EOF

# Parameters explained:
# model - LlamaGuard 4 (moderates image content with vision)
# Response: "safe" or unsafe category
# Use to filter inappropriate images before processing
