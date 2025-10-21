#!/usr/bin/env node
/**
 * 06_image_safety_check.js - Check if image content is safe using LlamaGuard Vision
 *
 * This example demonstrates how to:
 * 1. Load and encode an image in base64
 * 2. Send it to LlamaGuard for safety analysis
 * 3. Parse the moderation response
 *
 * LlamaGuard checks images for:
 * - Violence and graphic content
 * - Sexual content
 * - Hate symbols or offensive imagery
 * - Self-harm content
 * - Dangerous or illegal activities
 *
 * Usage:
 *     node 06_image_safety_check.js
 *
 * Environment:
 *     GROQ_API_KEY - Your Groq API key
 */

const https = require('https');
const fs = require('fs');
const path = require('path');

/**
 * Check if an image is safe using LlamaGuard Vision
 * @param {string} imagePath - Path to the image file
 * @returns {Promise<object>} API response with safety assessment
 */
async function checkImageSafety(imagePath) {
    // Step 1: Check API key
    const apiKey = process.env.GROQ_API_KEY;
    if (!apiKey) {
        throw new Error('GROQ_API_KEY environment variable not set');
    }

    // Step 2: Load and encode the image
    console.log(`Loading image: ${imagePath}`);

    // Check if file exists
    if (!fs.existsSync(imagePath)) {
        throw new Error(`Image not found: ${imagePath}`);
    }

    // Read the image file
    const imageData = fs.readFileSync(imagePath);

    // Encode to base64
    const imageBase64 = imageData.toString('base64');

    // Get image size for info
    const imageSize = imageData.length;
    console.log(`Image size: ${imageSize.toLocaleString()} bytes`);
    console.log(`Base64 size: ${imageBase64.length.toLocaleString()} characters`);

    // Step 3: Prepare the API request
    const requestData = {
        model: "meta-llama/llama-guard-4-12b",
        messages: [
            {
                role: "user",
                content: [
                    {
                        type: "text",
                        text: "Check this image for safety"
                    },
                    {
                        type: "image_url",
                        image_url: {
                            url: `data:image/jpeg;base64,${imageBase64}`
                        }
                    }
                ]
            }
        ],
        max_tokens: 100  // Safety responses are typically short
    };

    // Step 4: Make the API request
    console.log('\nChecking image safety with LlamaGuard...');

    return new Promise((resolve, reject) => {
        const options = {
            hostname: 'api.groq.com',
            port: 443,
            path: '/openai/v1/chat/completions',
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${apiKey}`,
                'Content-Type': 'application/json'
            }
        };

        const req = https.request(options, (res) => {
            let data = '';

            res.on('data', (chunk) => {
                data += chunk;
            });

            res.on('end', () => {
                try {
                    const result = JSON.parse(data);

                    // Check for API errors
                    if (result.error) {
                        reject(new Error(`API Error: ${result.error.message}`));
                    } else {
                        resolve(result);
                    }
                } catch (e) {
                    reject(new Error(`Failed to parse response: ${e.message}`));
                }
            });
        });

        req.on('error', (e) => {
            reject(new Error(`Request failed: ${e.message}`));
        });

        // Send the request
        req.write(JSON.stringify(requestData));
        req.end();
    });
}

/**
 * Interpret the LlamaGuard safety check result
 * @param {object} result - API response object
 * @returns {object} Interpretation with is_safe, category, and explanation
 */
function interpretSafetyResult(result) {
    // Check if we have a valid response
    if (!result.choices || result.choices.length === 0) {
        return {
            isSafe: false,
            category: 'error',
            explanation: 'No response from safety check'
        };
    }

    // Get the AI's response
    const content = result.choices[0].message.content.toLowerCase();

    // LlamaGuard typically returns "safe" or describes the unsafe category
    const isSafe = content.startsWith('safe');

    // Parse the category if unsafe
    if (!isSafe) {
        // Common unsafe categories from LlamaGuard
        const categories = {
            'violence': 'Violence or graphic content',
            'sexual': 'Sexual or suggestive content',
            'hate': 'Hate speech or discriminatory content',
            'self-harm': 'Self-harm or suicide content',
            'illegal': 'Illegal or dangerous activities',
            'deception': 'Deceptive or misleading content',
            'privacy': 'Privacy violation',
            'children': 'Child safety concern'
        };

        // Find which category was flagged
        for (const [key, description] of Object.entries(categories)) {
            if (content.includes(key)) {
                return {
                    isSafe: false,
                    category: key,
                    explanation: description
                };
            }
        }

        // Generic unsafe
        return {
            isSafe: false,
            category: 'unsafe',
            explanation: content
        };
    }

    return {
        isSafe: true,
        category: 'safe',
        explanation: 'Image passed safety checks'
    };
}

/**
 * Main function to demonstrate image safety checking
 */
async function main() {
    // Example image paths to check
    const testImages = [
        'test_image.jpg',         // Default test image
        '../test_image.jpg',      // Try parent directory
        'image.jpg'               // Alternative name
    ];

    // Find first available image
    let imagePath = null;
    for (const testPath of testImages) {
        if (fs.existsSync(testPath)) {
            imagePath = testPath;
            break;
        }
    }

    if (!imagePath) {
        console.log('No test image found. Please provide an image file.');
        console.log('Expected one of:', testImages.join(', '));
        return;
    }

    try {
        // Check image safety
        const result = await checkImageSafety(imagePath);

        // Interpret results
        const { isSafe, category, explanation } = interpretSafetyResult(result);

        // Display results
        console.log('\n' + '='.repeat(50));
        console.log('SAFETY CHECK RESULTS');
        console.log('='.repeat(50));
        console.log(`Image: ${imagePath}`);
        console.log(`Status: ${isSafe ? '✅ SAFE' : '⚠️ UNSAFE'}`);
        console.log(`Category: ${category}`);
        console.log(`Details: ${explanation}`);

        // Show raw response
        if (result.choices && result.choices.length > 0) {
            console.log(`\nRaw response: ${result.choices[0].message.content}`);
        }

        // Display usage statistics
        if (result.usage) {
            const usage = result.usage;
            console.log('\n' + '='.repeat(50));
            console.log('USAGE STATISTICS');
            console.log('='.repeat(50));
            console.log(`Prompt tokens: ${(usage.prompt_tokens || 0).toLocaleString()}`);
            console.log(`Completion tokens: ${(usage.completion_tokens || 0).toLocaleString()}`);
            console.log(`Total tokens: ${(usage.total_tokens || 0).toLocaleString()}`);

            // Calculate approximate cost (LlamaGuard pricing)
            // $0.20 per 1M tokens (both input and output)
            const totalTokens = usage.total_tokens || 0;
            const cost = (totalTokens / 1_000_000) * 0.20;
            console.log(`Estimated cost: $${cost.toFixed(6)}`);
        }

        // Educational note
        console.log('\n' + '='.repeat(50));
        console.log('💡 EDUCATIONAL NOTES');
        console.log('='.repeat(50));
        console.log('- LlamaGuard can analyze images for multiple safety categories');
        console.log('- Always check images before processing in production apps');
        console.log('- Consider caching results for frequently checked images');
        console.log('- Combine with text safety checks for complete moderation');
        console.log('- Image encoding increases data size by ~33% (base64)');

    } catch (error) {
        console.error(`Error checking image safety: ${error.message}`);
    }
}

// Run the main function
if (require.main === module) {
    main().catch(console.error);
}