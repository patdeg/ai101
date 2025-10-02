#!/bin/sh
# Analyze an image with AI vision
# Usage: ./03_vision_minimal.sh

# Base64 encode an image (replace with your image path)
IMAGE_BASE64=$(base64 -w 0 image.jpg 2>/dev/null || base64 -i image.jpg)

curl -s https://api.groq.com/openai/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d @- <<EOF
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
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
# model - llama-4-scout-17b-16e-instruct (supports vision)
# content array - Mix of text and image_url types
# image_url - Base64-encoded image data URI
