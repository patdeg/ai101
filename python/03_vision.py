#!/usr/bin/env python3
"""
Example 3: Vision - Analyzing Images
The AI can "see" and analyze images using base64 encoding
"""

import http.client
import json
import os
import base64

# Check for API key
api_key = os.environ.get('GROQ_API_KEY')
if not api_key:
    print("Error: GROQ_API_KEY not set")
    exit(1)

# Path to your image file
image_path = './test_image.jpg'

# Read and encode the image
try:
    with open(image_path, 'rb') as image_file:
        image_data = image_file.read()
        image_base64 = base64.b64encode(image_data).decode('utf-8')

    print(f"Image loaded: {image_path}")
    print(f"Image size: {len(image_data) / 1024:.2f} KB")
    print(f"Base64 size: {len(image_base64) / 1024:.2f} KB")
    print("(Base64 is ~33% larger than original)\n")

except FileNotFoundError:
    print(f"Error: Image file '{image_path}' not found")
    print("\nTo create a test image, run:")
    print("  curl -o test_image.jpg https://picsum.photos/400/300")
    exit(1)

# Detect image format from file extension
if image_path.lower().endswith('.png'):
    mime_type = 'image/png'
elif image_path.lower().endswith(('.jpg', '.jpeg')):
    mime_type = 'image/jpeg'
elif image_path.lower().endswith('.gif'):
    mime_type = 'image/gif'
elif image_path.lower().endswith('.webp'):
    mime_type = 'image/webp'
else:
    mime_type = 'image/jpeg'  # default

# Build the data URL
image_url = f"data:{mime_type};base64,{image_base64}"

# Create the request
payload = {
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
        {
            "role": "user",
            "content": [
                {
                    "type": "text",
                    "text": "What is in this image? Describe it in detail."
                },
                {
                    "type": "image_url",
                    "image_url": {
                        "url": image_url
                    }
                }
            ]
        }
    ],
    "temperature": 0.3,
    "max_tokens": 500
}

conn = http.client.HTTPSConnection("api.groq.com")
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

conn.request("POST", "/openai/v1/chat/completions", json.dumps(payload), headers)
response = conn.getresponse()
response_data = json.loads(response.read().decode('utf-8'))
conn.close()

# Display results
print("AI Vision Analysis:")
print("=" * 50)
print(response_data['choices'][0]['message']['content'])
print("=" * 50)

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
