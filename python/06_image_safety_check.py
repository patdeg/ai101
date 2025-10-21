#!/usr/bin/env python3
"""
06_image_safety_check.py - Check if image content is safe using LlamaGuard Vision

This example demonstrates how to:
1. Load and encode an image in base64
2. Send it to LlamaGuard for safety analysis
3. Parse the moderation response

LlamaGuard checks images for:
- Violence and graphic content
- Sexual content
- Hate symbols or offensive imagery
- Self-harm content
- Dangerous or illegal activities

Usage:
    python 06_image_safety_check.py

Environment:
    GROQ_API_KEY - Your Groq API key
"""

import os
import json
import base64
import http.client
from pathlib import Path

def check_image_safety(image_path):
    """
    Check if an image is safe using LlamaGuard Vision

    Args:
        image_path: Path to the image file

    Returns:
        dict: API response with safety assessment
    """
    # Step 1: Check API key
    api_key = os.getenv('GROQ_API_KEY')
    if not api_key:
        raise ValueError("GROQ_API_KEY environment variable not set")

    # Step 2: Load and encode the image
    print(f"Loading image: {image_path}")

    # Check if file exists
    if not Path(image_path).exists():
        raise FileNotFoundError(f"Image not found: {image_path}")

    # Read and encode to base64
    with open(image_path, 'rb') as f:
        image_data = f.read()

    # Encode to base64 and convert to string
    image_base64 = base64.b64encode(image_data).decode('utf-8')

    # Get image size for info
    image_size = len(image_data)
    print(f"Image size: {image_size:,} bytes")
    print(f"Base64 size: {len(image_base64):,} characters")

    # Step 3: Prepare the API request
    request_data = {
        "model": "meta-llama/llama-guard-4-12b",
        "messages": [
            {
                "role": "user",
                "content": [
                    {
                        "type": "text",
                        "text": "Check this image for safety"
                    },
                    {
                        "type": "image_url",
                        "image_url": {
                            "url": f"data:image/jpeg;base64,{image_base64}"
                        }
                    }
                ]
            }
        ],
        "max_tokens": 100  # Safety responses are typically short
    }

    # Step 4: Make the API request
    print("\nChecking image safety with LlamaGuard...")

    conn = http.client.HTTPSConnection("api.groq.com")

    headers = {
        'Authorization': f'Bearer {api_key}',
        'Content-Type': 'application/json'
    }

    # Convert request to JSON
    json_data = json.dumps(request_data)

    # Send request
    conn.request("POST", "/openai/v1/chat/completions", json_data, headers)

    # Get response
    response = conn.getresponse()
    response_data = response.read().decode()

    # Step 5: Parse the response
    result = json.loads(response_data)

    # Check for errors
    if 'error' in result:
        raise Exception(f"API Error: {result['error']['message']}")

    return result

def interpret_safety_result(result):
    """
    Interpret the LlamaGuard safety check result

    Args:
        result: API response dictionary

    Returns:
        tuple: (is_safe, category, explanation)
    """
    if 'choices' not in result or not result['choices']:
        return False, "error", "No response from safety check"

    # Get the AI's response
    content = result['choices'][0]['message']['content'].lower()

    # LlamaGuard typically returns "safe" or describes the unsafe category
    is_safe = content.startswith("safe")

    # Parse the category if unsafe
    if not is_safe:
        # Common unsafe categories from LlamaGuard
        categories = {
            "violence": "Violence or graphic content",
            "sexual": "Sexual or suggestive content",
            "hate": "Hate speech or discriminatory content",
            "self-harm": "Self-harm or suicide content",
            "illegal": "Illegal or dangerous activities",
            "deception": "Deceptive or misleading content",
            "privacy": "Privacy violation",
            "children": "Child safety concern"
        }

        # Find which category was flagged
        for key, description in categories.items():
            if key in content:
                return False, key, description

        # Generic unsafe
        return False, "unsafe", content

    return True, "safe", "Image passed safety checks"

def main():
    """Main function to demonstrate image safety checking"""

    # Example image paths to check
    test_images = [
        "test_image.jpg",  # Default test image
        "../test_image.jpg",  # Try parent directory
        "image.jpg"  # Alternative name
    ]

    # Find first available image
    image_path = None
    for path in test_images:
        if Path(path).exists():
            image_path = path
            break

    if not image_path:
        print("No test image found. Please provide an image file.")
        print("Expected one of:", ", ".join(test_images))
        return

    try:
        # Check image safety
        result = check_image_safety(image_path)

        # Interpret results
        is_safe, category, explanation = interpret_safety_result(result)

        # Display results
        print("\n" + "="*50)
        print("SAFETY CHECK RESULTS")
        print("="*50)
        print(f"Image: {image_path}")
        print(f"Status: {'✅ SAFE' if is_safe else '⚠️ UNSAFE'}")
        print(f"Category: {category}")
        print(f"Details: {explanation}")

        # Show raw response
        if 'choices' in result and result['choices']:
            print(f"\nRaw response: {result['choices'][0]['message']['content']}")

        # Display usage statistics
        if 'usage' in result:
            usage = result['usage']
            print("\n" + "="*50)
            print("USAGE STATISTICS")
            print("="*50)
            print(f"Prompt tokens: {usage.get('prompt_tokens', 0):,}")
            print(f"Completion tokens: {usage.get('completion_tokens', 0):,}")
            print(f"Total tokens: {usage.get('total_tokens', 0):,}")

            # Calculate approximate cost (LlamaGuard pricing)
            # $0.20 per 1M tokens (both input and output)
            total_tokens = usage.get('total_tokens', 0)
            cost = (total_tokens / 1_000_000) * 0.20
            print(f"Estimated cost: ${cost:.6f}")

        # Educational note
        print("\n" + "="*50)
        print("💡 EDUCATIONAL NOTES")
        print("="*50)
        print("- LlamaGuard can analyze images for multiple safety categories")
        print("- Always check images before processing in production apps")
        print("- Consider caching results for frequently checked images")
        print("- Combine with text safety checks for complete moderation")

    except FileNotFoundError as e:
        print(f"Error: {e}")
        print("Please ensure you have an image file to test")
    except Exception as e:
        print(f"Error checking image safety: {e}")
        return

if __name__ == "__main__":
    main()