#!/usr/bin/env node
/**
 * Example 09: Tavily Extract API - Extract Clean Content from Web Pages
 *
 * What is Tavily Extract?
 * ----------------------
 * Converts web pages into clean, structured markdown:
 * - Removes ads, navigation, and clutter
 * - Extracts main content in markdown format
 * - Optionally extracts images
 * - Perfect for feeding content to LLMs
 *
 * This example:
 * ------------
 * - Extracts content from a Medium article about California AI rules
 * - Uses "advanced" extraction depth
 * - Includes images from the article
 * - Saves clean markdown output to file
 */

const https = require('https');
const fs = require('fs');

// Check for API key
if (!process.env.TAVILY_API_KEY) {
  console.error('Error: TAVILY_API_KEY environment variable not set\n');
  console.error('Get your free API key at: https://tavily.com');
  console.error('Then: export TAVILY_API_KEY="tvly-your-key-here"\n');
  process.exit(1);
}

const articleUrl = 'https://medium.com/@pdeglon/california-ai-rules-explained-in-everyday-english-fea55637cb96';

const requestData = JSON.stringify({
  urls: [articleUrl],
  include_images: true,
  extract_depth: 'advanced'
});

const options = {
  hostname: 'api.tavily.com',
  port: 443,
  path: '/extract',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.TAVILY_API_KEY}`,
    'Content-Length': Buffer.byteLength(requestData)
  }
};

console.log('===========================================');
console.log('Tavily Extract API - Medium Article');
console.log('===========================================\n');
console.log(`URL: ${articleUrl}`);
console.log('Options: extract_depth=advanced, include_images=true\n');
console.log('Sending request to Tavily...\n');

const req = https.request(options, (res) => {
  let data = '';

  res.on('data', (chunk) => {
    data += chunk;
  });

  res.on('end', () => {
    try {
      const response = JSON.parse(data);

      // Check for errors
      if (response.error) {
        console.error('API Error:', response.error);
        process.exit(1);
      }

      // Check extraction results
      const resultCount = response.results.length;
      const failedCount = response.failed_results.length;

      if (resultCount === 0) {
        console.error('Extraction failed\n');
        console.error('Failed URLs:', response.failed_results);
        process.exit(1);
      }

      console.log('========================================');
      console.log('Extraction Summary');
      console.log('========================================\n');
      console.log(`Successful: ${resultCount}`);
      console.log(`Failed: ${failedCount}\n`);

      // Extract content
      const content = response.results[0].raw_content;
      const contentLength = content.length;

      console.log('========================================');
      console.log('Extracted Content (Markdown)');
      console.log('========================================\n');
      console.log(`Content length: ${contentLength} characters\n`);
      console.log('First 1000 characters:\n');
      console.log(content.substring(0, 1000));
      console.log('\n... (truncated, see full output below) ...\n');

      // Extract images
      const images = response.results[0].images || [];
      const imageCount = images.length;

      console.log('========================================');
      console.log('Extracted Images');
      console.log('========================================\n');
      console.log(`Found ${imageCount} images:\n`);

      images.forEach((imageUrl, index) => {
        console.log(`[${index + 1}] ${imageUrl}`);
      });
      console.log('');

      // Display metadata
      console.log('========================================');
      console.log('Request Metadata');
      console.log('========================================\n');
      console.log(`Response Time: ${response.response_time}s`);
      console.log(`Content Size: ${contentLength} characters`);
      console.log(`Images Extracted: ${imageCount}\n`);

      // Save full content to file
      const outputFile = 'extracted_content.md';
      fs.writeFileSync(outputFile, content);

      console.log('========================================');
      console.log('Full Content Saved');
      console.log('========================================\n');
      console.log(`Saved to: ${outputFile}\n`);
      console.log(`View with: cat ${outputFile}`);
      console.log(`Or open with your editor\n`);

      // Display raw JSON
      console.log('========================================');
      console.log('Raw API Response (formatted)');
      console.log('========================================\n');
      console.log(JSON.stringify(response, null, 2));
      console.log('');

      // Summary
      console.log('===========================================');
      console.log('Summary');
      console.log('===========================================\n');
      console.log('✓ Content extracted successfully');
      console.log(`✓ Extracted ${contentLength} characters of clean markdown`);
      console.log(`✓ Found ${imageCount} images`);
      console.log(`✓ Saved to ${outputFile}`);
      console.log(`✓ Response time: ${response.response_time}s\n`);
      console.log('Try the exercises to:');
      console.log('  - Extract content from multiple URLs at once');
      console.log('  - Compare basic vs. advanced extraction depth');
      console.log('  - Feed extracted content to AI for summarization');
      console.log('  - Build a web clipper / read-later service\n');

    } catch (error) {
      console.error('Error parsing response:', error.message);
      console.error('Raw response:', data);
      process.exit(1);
    }
  });
});

req.on('error', (error) => {
  console.error('Request error:', error.message);
  process.exit(1);
});

req.write(requestData);
req.end();
