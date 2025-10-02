#!/bin/bash

################################################################################
# EXAMPLE 5: IMAGE SAFETY CHECK WITH LLAMAGUARD VISION
################################################################################
#
# What this demonstrates:
#   - How to check images for harmful content using LlamaGuard Vision
#   - Base64 encoding images for API transmission
#   - Combining image analysis with safety moderation
#
# What you'll learn:
#   - LlamaGuard can analyze BOTH text and images
#   - Same 14 safety categories apply to visual content
#   - How to moderate user-uploaded images before processing
#   - Building multi-modal safety pipelines
#
# Prerequisites:
#   - GROQ_API_KEY environment variable must be set
#   - jq command installed (see README.md for installation)
#   - base64 command (pre-installed on Linux/Mac)
#   - test_image.jpg in parent directory
#
# Expected output:
#   - Safety verdict: "safe" or "unsafe\nS<number>"
#   - User-friendly interpretation
#   - Raw API response
#
################################################################################

# Store the API endpoint URL
API_URL="https://api.groq.com/openai/v1/chat/completions"

# STEP 1: Set the path to the image we want to check for safety
# Using the shared test image from the root directory
IMAGE_PATH="../test_image.jpg"

# Check if the image exists
if [ ! -f "$IMAGE_PATH" ]; then
  echo "Error: Image not found at $IMAGE_PATH"
  echo "Please make sure test_image.jpg exists in the root directory"
  exit 1
fi

# STEP 2: Convert the image to base64 format
# This is required because JSON can't contain raw binary data
echo "Converting image to base64 format..."
IMAGE_BASE64=$(base64 -w 0 "$IMAGE_PATH" 2>/dev/null || base64 "$IMAGE_PATH")

# STEP 3: Build the data URL
# Format: data:image/jpeg;base64,<encoded_image_data>
IMAGE_URL="data:image/jpeg;base64,$IMAGE_BASE64"

# STEP 4: Send the image to LlamaGuard for safety analysis
# LlamaGuard 4 supports BOTH text and images
echo "Checking image for safety violations..."
echo ""
RESPONSE=$(curl -s "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-guard-4-12b",
    "messages": [
      {
        "role": "user",
        "content": [
          {
            "type": "image_url",
            "image_url": {
              "url": "'"$IMAGE_URL"'"
            }
          }
        ]
      }
    ],
    "max_tokens": 100
  }')

# STEP 5: Display the safety verdict
echo "========================================"
echo "Image Safety Check Result:"
echo "========================================"
VERDICT=$(echo "$RESPONSE" | jq -r '.choices[0].message.content')
echo "$VERDICT"

# STEP 6: Parse and display the verdict in a user-friendly way
echo ""
echo "========================================"
echo "Interpretation:"
echo "========================================"
if [[ "$VERDICT" == "safe" ]]; then
  echo "✓ Image is SAFE to process"
  echo "  No harmful visual content detected"
  echo ""
  echo "  The image passed all 14 safety categories:"
  echo "  - No violent crimes imagery"
  echo "  - No illegal activity"
  echo "  - No sexual content"
  echo "  - No hateful symbols"
  echo "  - No weapons or dangerous items"
  echo "  - No self-harm imagery"
  echo "  - And more..."
else
  # Extract the category (e.g., "S12" from "unsafe\nS12")
  CATEGORY=$(echo "$VERDICT" | tail -1)
  echo "✗ Image is UNSAFE"
  echo "  Violation category: $CATEGORY"
  echo ""
  echo "  Category meanings:"
  echo "  S1  = Violent Crimes (graphic violence, weapons)"
  echo "  S2  = Non-Violent Crimes (illegal activity)"
  echo "  S3  = Sex-Related Crimes"
  echo "  S4  = Child Sexual Exploitation"
  echo "  S5  = Defamation"
  echo "  S6  = Specialized Advice"
  echo "  S7  = Privacy Violations (personal info visible)"
  echo "  S8  = Intellectual Property"
  echo "  S9  = Indiscriminate Weapons (CBRNE)"
  echo "  S10 = Hate Speech (hateful symbols, gestures)"
  echo "  S11 = Suicide & Self-Harm (self-harm imagery)"
  echo "  S12 = Sexual Content (nudity, sexual acts)"
  echo "  S13 = Elections (misleading political imagery)"
  echo "  S14 = Code Interpreter Abuse"
  echo ""
  echo "  ⚠ ACTION: Do not process this image further"
  echo "  Consider logging the attempt and blocking the user"
fi

echo ""
echo "========================================"
echo "Raw API Response:"
echo "========================================"
echo "$RESPONSE" | jq .
echo ""

################################################################################
# EXPLANATION OF THE API REQUEST
################################################################################
#
# What's different from text safety checks?
#
# Content structure:
#   Instead of { "type": "text", "text": "..." }
#   We use:   { "type": "image_url", "image_url": { "url": "..." } }
#
# The model:
#   "meta-llama/llama-guard-4-12b" supports BOTH text and images
#   It can analyze visual content for the same 14 categories
#
# Use cases for image safety:
#   - User profile pictures
#   - User-uploaded content (forums, social media)
#   - Generated images (before displaying to users)
#   - Document scanning (checking for sensitive info)
#   - Content moderation pipelines
#
# Production workflow:
#   1. User uploads image
#   2. Check with LlamaGuard Vision (this example)
#   3. If "safe" → proceed with storage/processing
#   4. If "unsafe" → reject and explain category
#   5. Log all violations for review
#
# Combined safety pipeline:
#   ┌─────────────────┐
#   │ User Input      │ (text + image)
#   └────────┬────────┘
#            │
#            ▼
#   ┌─────────────────┐
#   │ Prompt Guard    │ ← Check text for injection (Example 6)
#   └────────┬────────┘
#            │ BENIGN
#            ▼
#   ┌─────────────────┐
#   │ LlamaGuard Text │ ← Check text for safety (Example 4)
#   └────────┬────────┘
#            │ safe
#            ▼
#   ┌─────────────────┐
#   │ LlamaGuard      │ ← Check image for safety (THIS EXAMPLE)
#   │ Vision          │
#   └────────┬────────┘
#            │ safe
#            ▼
#   ┌─────────────────┐
#   │ Process Request │ ← All checks passed!
#   └─────────────────┘
#
# Why this matters:
#   - Users can upload harmful images (intentionally or not)
#   - You're responsible for content on your platform
#   - Automated moderation scales better than human review
#   - Prevents illegal content from being stored/distributed
#
# Image-specific violations to watch for:
#   - S1: Graphic violence, crime scene photos
#   - S7: Screenshots with personal info (IDs, credit cards)
#   - S10: Hate symbols (swastikas, hate gestures)
#   - S11: Self-harm imagery
#   - S12: Nudity, sexual content
#   - S13: Fake/misleading political images
#
# Testing this example:
#   The default test_image.jpg should be safe
#   To test unsafe detection, you could:
#   - Use a stock photo with warning labels
#   - Create a test image with text like "UNSAFE CONTENT TEST"
#   - Download sample images from content moderation datasets
#
# Cost consideration:
#   Images use more tokens than text
#   A typical image = 500-2000 tokens depending on size
#   At $0.20 per 1M input tokens, most checks cost < $0.001
