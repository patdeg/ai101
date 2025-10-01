// Example 3: Vision - Analyzing Images
// The AI can "see" and analyze images

const https = require('https');
const fs = require('fs');
const path = require('path');

// Path to your image (change this to your image file)
const imagePath = './test_image.jpg';

// Read image file and convert to base64
let imageBase64;
try {
  const imageBuffer = fs.readFileSync(imagePath);
  imageBase64 = imageBuffer.toString('base64');
  console.log(`Image loaded: ${imagePath}`);
  console.log(`Image size: ${(imageBuffer.length / 1024).toFixed(2)} KB`);
  console.log(`Base64 size: ${(imageBase64.length / 1024).toFixed(2)} KB`);
  console.log('(Base64 is ~33% larger than original)\n');
} catch (error) {
  console.error(`Error reading image: ${error.message}`);
  console.log('\nTo create a test image, run:');
  console.log('  curl -o test_image.jpg https://picsum.photos/400/300');
  process.exit(1);
}

// Detect image type from file extension
const ext = path.extname(imagePath).toLowerCase();
const mimeTypes = {
  '.jpg': 'image/jpeg',
  '.jpeg': 'image/jpeg',
  '.png': 'image/png',
  '.gif': 'image/gif',
  '.webp': 'image/webp'
};
const mimeType = mimeTypes[ext] || 'image/jpeg';

// Build the data URL
const imageUrl = `data:${mimeType};base64,${imageBase64}`;

// Create request with image
const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",
  messages: [
    {
      role: "user",
      content: [
        {
          type: "text",
          text: "What is in this image? Describe it in detail."
        },
        {
          type: "image_url",
          image_url: {
            url: imageUrl
          }
        }
      ]
    }
  ],
  temperature: 0.3,
  max_tokens: 500
});

const options = {
  hostname: 'api.groq.com',
  port: 443,
  path: '/openai/v1/chat/completions',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.GROQ_API_KEY}`,
    'Content-Length': Buffer.byteLength(data)
  }
};

const req = https.request(options, (res) => {
  let responseData = '';

  res.on('data', (chunk) => {
    responseData += chunk;
  });

  res.on('end', () => {
    try {
      const parsed = JSON.parse(responseData);

      console.log('AI Vision Analysis:');
      console.log('='.repeat(50));
      console.log(parsed.choices[0].message.content);
      console.log('='.repeat(50));

      console.log('\nToken Usage:');
      console.log(`Total: ${parsed.usage.total_tokens} tokens`);

    } catch (error) {
      console.error('Error:', error.message);

      // Check for common errors
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

req.on('error', (error) => {
  console.error('Request error:', error.message);
});

req.write(data);
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
