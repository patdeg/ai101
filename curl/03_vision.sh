#!/bin/bash

################################################################################
# EXAMPLE 3: VISION - ANALYZING IMAGES
################################################################################
#
# What this demonstrates:
#   - How to send images to the AI for analysis
#   - Converting images to base64 format for API transmission
#   - Combining text and images in a single request
#
# What you'll learn:
#   - Images must be base64-encoded before sending
#   - Content can be an array of multiple types (text + image)
#   - How to use data URLs for embedding images
#
# Prerequisites:
#   - GROQ_API_KEY environment variable must be set
#   - An image file (test_image.jpg) in the current directory
#   - base64 command (pre-installed on Linux/Mac)
#
# Expected output:
#   - Detailed description of what's in the image
#   - Objects, colors, people, text, and context identified
#
# To create a test image if you don't have one:
#   curl -o test_image.jpg https://picsum.photos/400/300
#
################################################################################

# Store the API endpoint URL
API_URL="https://api.groq.com/openai/v1/chat/completions"

# STEP 1: Set the path to the image we want to analyze
# Using the shared test image from the root directory
IMAGE_PATH="../test_image.jpg"

# STEP 2: Convert the image to base64 format
# Why? Because JSON can't contain raw binary image data
# The -w 0 flag means "no line wrapping" - we need one continuous string
echo "Converting image to base64 format..."
IMAGE_BASE64=$(base64 -w 0 "$IMAGE_PATH")

# STEP 3: Build the data URL in the required format
# Format: data:image/jpeg;base64,<encoded_image_data>
# Change "image/jpeg" to "image/png" if using PNG files
IMAGE_URL="data:image/jpeg;base64,$IMAGE_BASE64"

# STEP 4: Send the request with both text and image
# Note: content is now an ARRAY containing multiple items
echo "Sending image to AI for analysis..."
curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "user",
        "content": [                              # Content is an array, not a string!
          {
            "type": "text",                       # First item: our question
            "text": "What is in this image? Describe in detail."
          },
          {
            "type": "image_url",                  # Second item: the image to analyze
            "image_url": {
              "url": "'"$IMAGE_URL"'"             # Insert our base64 image data URL
            }
          }
        ]
      }
    ],
    "temperature": 0.3,                           # Lower = more accurate descriptions
    "max_tokens": 500                             # Allow longer response for details
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
