#!/usr/bin/env python3
"""
Example 3: Vision - AI Image Analysis with Base64 Encoding

WHAT THIS DEMONSTRATES:
    - How to enable AI to "see" and analyze images
    - Reading binary image files in Python
    - Base64 encoding for embedding images in JSON
    - Multi-modal content (text + image) in a single request
    - Automatic image format detection

WHAT YOU'LL LEARN:
    - Binary file handling with 'rb' mode
    - Base64 encoding/decoding for images
    - MIME type detection from file extensions
    - Data URL format for embedded images
    - Multi-content message structure
    - Error handling with try/except

PREREQUISITES:
    - Python 3.6 or higher
    - GROQ_API_KEY environment variable set
    - A test image file (test_image.jpg)
    - Vision-capable AI model

EXPECTED OUTPUT:
    - Image file information (path, size, encoding size)
    - Detailed AI analysis of the image content
    - Token usage statistics

Run with: python3 03_vision.py
Create test image: curl -o test_image.jpg https://picsum.photos/400/300
"""

import http.client  # For HTTPS API requests
import json         # For JSON encoding/decoding
import os           # For environment variables
import base64       # For encoding binary image data to text

# ==============================================================================
# Step 1: Load and validate API credentials
# ==============================================================================

# Retrieve API key from environment variable
api_key = os.environ.get('GROQ_API_KEY')

# Validate API key exists
if not api_key:
    print("Error: GROQ_API_KEY not set")
    exit(1)

# ==============================================================================
# Step 2: Specify the image file path
# ==============================================================================

# Path to test_image.jpg from the root directory
image_path = '../test_image.jpg'

# ==============================================================================
# Step 3: Read and encode the image file
# ==============================================================================

# Before sending images to the API, we must:
# 1. Read the image as binary data
# 2. Encode it to base64 (converts binary to text)
# 3. Create a data URL with the base64 string

# Try to read and encode the image (with error handling)
try:
    # Open file in binary read mode ('rb') - images are binary data, not text
    with open(image_path, 'rb') as image_file:
        # Read the entire image file into memory as bytes
        image_data = image_file.read()

        # Before sending to API, encode binary data to base64 string
        # base64.b64encode() returns bytes, so we decode to get a string
        image_base64 = base64.b64encode(image_data).decode('utf-8')

    # Display image loading information
    print(f"Image loaded: {image_path}")
    print(f"Image size: {len(image_data) / 1024:.2f} KB")
    print(f"Base64 size: {len(image_base64) / 1024:.2f} KB")
    print("(Base64 is ~33% larger than original)\n")

except FileNotFoundError:
    # Handle case where image file doesn't exist
    print(f"Error: Image file '{image_path}' not found")
    print("\nTo create a test image, run:")
    print("  curl -o test_image.jpg https://picsum.photos/400/300")
    exit(1)

# ==============================================================================
# Step 4: Detect image MIME type from file extension
# ==============================================================================

# Before creating the data URL, we need to specify the correct MIME type
# This tells the API what kind of image we're sending

# Check file extension to determine MIME type
if image_path.lower().endswith('.png'):
    mime_type = 'image/png'
elif image_path.lower().endswith(('.jpg', '.jpeg')):
    mime_type = 'image/jpeg'
elif image_path.lower().endswith('.gif'):
    mime_type = 'image/gif'
elif image_path.lower().endswith('.webp'):
    mime_type = 'image/webp'
else:
    mime_type = 'image/jpeg'  # Default to JPEG if unknown

# ==============================================================================
# Step 5: Build the data URL with base64-encoded image
# ==============================================================================

# Create data URL format: data:[mime-type];base64,[base64-data]
# This is how we embed the image directly in JSON
image_url = f"data:{mime_type};base64,{image_base64}"

# ==============================================================================
# Step 6: Create the request payload with multi-modal content
# ==============================================================================

# Before building the payload, understand the content array structure:
# - Each user message can contain multiple content items
# - Content items can be text OR images
# - The AI processes them together

# Create the request with BOTH text and image content
payload = {
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
        {
            "role": "user",
            # IMPORTANT: content is now an ARRAY of content items, not a string
            "content": [
                {
                    # First content item: text instruction
                    "type": "text",
                    "text": "What is in this image? Describe it in detail."
                },
                {
                    # Second content item: the image itself
                    "type": "image_url",
                    "image_url": {
                        "url": image_url  # Our base64-encoded data URL
                    }
                }
            ]
        }
    ],
    "temperature": 0.3,  # Low temperature for factual, detailed descriptions
    "max_tokens": 500    # Allow longer response for detailed image description
}

# ==============================================================================
# Step 7: Establish HTTPS connection to the API
# ==============================================================================

# Create secure connection to Groq API
conn = http.client.HTTPSConnection("api.groq.com")

# ==============================================================================
# Step 8: Prepare authentication headers
# ==============================================================================

# Set up required headers
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

# ==============================================================================
# Step 9: Send the POST request with image data
# ==============================================================================

# Make the API request (this may take longer due to image processing)
conn.request("POST", "/openai/v1/chat/completions", json.dumps(payload), headers)

# ==============================================================================
# Step 10: Receive and parse the response
# ==============================================================================

# Get the response from the server
response = conn.getresponse()

# Parse the JSON response
response_data = json.loads(response.read().decode('utf-8'))

# ==============================================================================
# Step 11: Clean up the connection
# ==============================================================================

# Close the connection
conn.close()

# ==============================================================================
# Step 12: Display the AI's image analysis
# ==============================================================================

# Display the AI's visual analysis of the image
print("AI Vision Analysis:")
print("=" * 50)
print(response_data['choices'][0]['message']['content'])
print("=" * 50)

# Show token usage (note: images consume more tokens than text)
print(f"\nToken Usage: {response_data['usage']['total_tokens']} tokens")

# Python file handling concepts:
#
# open(path, mode)
#   'r'  = read text
#   'rb' = read binary (for images, videos, etc.)
#   'w'  = write text (overwrites file)
#   'wb' = write binary
#   'a'  = append text
#
# with statement (context manager):
#   with open('file.txt', 'r') as f:
#       data = f.read()
#   # File automatically closed here (even if error occurs)
#
# Without with:
#   f = open('file.txt', 'r')
#   data = f.read()
#   f.close()  # Must remember to close!
#
# Reading files:
#   f.read()        # Read entire file as string
#   f.read(100)     # Read first 100 bytes
#   f.readline()    # Read one line
#   f.readlines()   # Read all lines into list
#
# File size:
#   os.path.getsize('file.txt')  # Size in bytes
#   len(data)                    # Size of data in memory
#
# Base64 encoding:
#   base64.b64encode(bytes)     # Encode bytes → base64 bytes
#   base64.b64decode(bytes)     # Decode base64 bytes → original bytes
#   .decode('utf-8')            # Convert bytes → string
#   .encode('utf-8')            # Convert string → bytes
#
# Example:
#   original = b'Hello'                    # bytes
#   encoded = base64.b64encode(original)   # b'SGVsbG8='
#   encoded_str = encoded.decode('utf-8')  # 'SGVsbG8='
#   decoded = base64.b64decode(encoded)    # b'Hello'
#
# String methods for paths:
#   .endswith('.jpg')     # True if string ends with '.jpg'
#   .startswith('/home')  # True if string starts with '/home'
#   .lower()              # Convert to lowercase
#   .replace('.jpg', '.png')  # Replace substring
#
# Better way using pathlib (Python 3.4+):
#   from pathlib import Path
#   p = Path('image.jpg')
#   p.suffix         # '.jpg'
#   p.stem           # 'image'
#   p.name           # 'image.jpg'
#   p.exists()       # True if file exists
#
# Image size limits:
#   Base64 embedded: 4 MB max
#   URL (remote): 20 MB max
#   Resolution: 33 megapixels max
#
# Why base64?
#   - JSON can't contain raw binary data
#   - Base64 converts binary → text
#   - Every 3 bytes → 4 characters
#   - Size increase: ~33%
#
# Content array structure:
#   Multiple content items in one message
#   [
#       {"type": "text", "text": "..."},
#       {"type": "image_url", "image_url": {...}}
#   ]
#
# Try different questions:
#   "What colors are dominant in this image?"
#   "Is there any text visible? If so, what does it say?"
#   "Describe this image for someone who can't see"
#   "What's the mood or atmosphere of this image?"
#   "Count how many people are in this image"
