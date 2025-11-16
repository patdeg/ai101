/**
 * ============================================================================
 * Example 3: Vision - Teaching AI to "See" Images
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to send images to AI models for analysis
 * - Converting images to base64 format for API transmission
 * - Working with file system operations in Node.js
 * - Multi-modal AI (text + images together)
 *
 * WHAT YOU'LL LEARN:
 * - Reading image files from disk
 * - Base64 encoding explained and why it's needed
 * - Creating data URLs for image transmission
 * - Detecting file types and MIME types
 * - Handling file system errors gracefully
 *
 * PREREQUISITES:
 * - Completed 01_basic_chat.js and 02_system_prompt.js
 * - DEMETERICS_API_KEY environment variable set
 * - A test image file (or use the curl command below to download one)
 *
 * EXPECTED OUTPUT:
 * - Image file information (size, base64 size)
 * - AI's detailed description of what's in the image
 * - Token usage statistics
 *
 * RUN THIS:
 * # First, create a test image:
 * curl -o test_image.jpg https://picsum.photos/400/300
 * # Then run:
 * node 03_vision.js
 * ============================================================================
 */

// Step 1: Import required modules
const https = require('https');  // For making API requests
const fs = require('fs');        // File System - for reading files
const path = require('path');    // For working with file paths

// Step 2: Define the path to the image you want to analyze
// Using test_image.jpg from the root directory
const imagePath = '../test_image.jpg';

// Step 3: Read the image file and convert it to base64 format
// Base64 is a way to represent binary data (images) as text
let imageBase64;
try {
  // Step 3a: Read the entire image file into memory as a Buffer (binary data)
  const imageBuffer = fs.readFileSync(imagePath);

  // Step 3b: Convert the binary Buffer to a base64 string
  // Base64 encoding allows binary data to be transmitted in JSON
  imageBase64 = imageBuffer.toString('base64');

  // Step 3c: Display image information for educational purposes
  console.log(`Image loaded: ${imagePath}`);
  console.log(`Image size: ${(imageBuffer.length / 1024).toFixed(2)} KB`);
  console.log(`Base64 size: ${(imageBase64.length / 1024).toFixed(2)} KB`);
  console.log('(Base64 is ~33% larger than original)\n');
} catch (error) {
  // Step 3d: Handle file not found or other errors
  console.error(`Error reading image: ${error.message}`);
  console.log('\nTo create a test image, run:');
  console.log('  curl -o test_image.jpg https://picsum.photos/400/300');
  process.exit(1);  // Exit with error code
}

// Step 4: Detect the image type from file extension
// This determines the correct MIME type for the data URL
const ext = path.extname(imagePath).toLowerCase();  // e.g., ".jpg"
const mimeTypes = {
  '.jpg': 'image/jpeg',
  '.jpeg': 'image/jpeg',
  '.png': 'image/png',
  '.gif': 'image/gif',
  '.webp': 'image/webp'
};
const mimeType = mimeTypes[ext] || 'image/jpeg';  // Default to jpeg if unknown

// Step 5: Build the data URL
// Format: data:<mime-type>;base64,<base64-encoded-data>
const imageUrl = `data:${mimeType};base64,${imageBase64}`;

// Step 6: Create the request payload with both text and image
// Notice the content is now an ARRAY of objects (not a string)
const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",
  messages: [
    {
      role: "user",
      content: [  // Content is an array for multi-modal input
        {
          type: "text",  // First element: the text prompt
          text: "What is in this image? Describe it in detail."
        },
        {
          type: "image_url",  // Second element: the image
          image_url: {
            url: imageUrl  // The data URL we created above
          }
        }
      ]
    }
  ],
  temperature: 0.3,  // Low temperature for consistent descriptions
  max_tokens: 500    // More tokens needed for detailed image descriptions
});

// Step 7: Configure the HTTPS request options
const options = {
  hostname: 'api.demeterics.com',
  port: 443,
  path: '/groq/v1/chat/completions',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.DEMETERICS_API_KEY}`,
    'Content-Length': Buffer.byteLength(data)
  }
};

// Step 8: Create and send the HTTPS request
const req = https.request(options, (res) => {
  // Step 9: Accumulate response data
  let responseData = '';

  // Step 10: Handle incoming data chunks
  res.on('data', (chunk) => {
    responseData += chunk;
  });

  // Step 11: Process the complete response
  res.on('end', () => {
    try {
      // Step 12: Parse the JSON response
      const parsed = JSON.parse(responseData);

      // Step 13: Display the AI's vision analysis
      console.log('AI Vision Analysis:');
      console.log('='.repeat(50));
      console.log(parsed.choices[0].message.content);
      console.log('='.repeat(50));

      // Step 14: Show token usage
      console.log('\nToken Usage:');
      console.log(`Total: ${parsed.usage.total_tokens} tokens`);

    } catch (error) {
      // Step 15: Handle parsing errors
      console.error('Error:', error.message);

      // Check for common image-related errors
      if (responseData.includes('image')) {
        console.log('\nImage might be too large. Try:');
        console.log('- Resize to under 4MB (for base64)');
        console.log('- Use JPEG instead of PNG');
        console.log('- Reduce image quality/resolution');
      }
      console.log('\nRaw response:', responseData);
    }
  });
});

// Step 16: Handle request errors
req.on('error', (error) => {
  console.error('Request error:', error.message);
});

// Step 17: Send the request data (includes the base64 image)
req.write(data);

// Step 18: Complete and send the request
req.end();

// Node.js file system concepts:
//
// require('fs')
//   File System module - for reading/writing files
//
// require('path')
//   Path module - for working with file paths
//
// fs.readFileSync(path)
//   Reads entire file into memory (synchronous = waits)
//   Returns a Buffer (binary data)
//   Use fs.readFile() for async (non-blocking)
//
// buffer.toString('base64')
//   Converts binary data to base64 text
//   Other options: 'utf8', 'hex', 'ascii'
//
// path.extname(filepath)
//   Gets file extension: '/path/to/photo.jpg' → '.jpg'
//
// process.exit(1)
//   Exits program with error code 1
//   0 = success, non-zero = error
//
// Base64 encoding:
//   Binary data → Text representation
//   Required because JSON can't contain raw binary
//   Size increase: ~33% larger than original
//   Formula: base64_size = Math.ceil(file_size * 4/3)
//
// Image size limits:
//   Base64 (embedded): 4 MB max
//   URL (remote): 20 MB max
//   Resolution: 33 megapixels max
//
// Content array structure:
//   [
//     { type: "text", text: "..." },        // Your question
//     { type: "image_url", image_url: {...} }  // The image
//   ]
//   Order doesn't matter - AI sees both
//
// Try different questions:
//   "What colors are in this image?"
//   "Is there any text? If so, what does it say?" (OCR)
//   "Describe this image for a blind person"
//   "What's the mood or emotion of this image?"
