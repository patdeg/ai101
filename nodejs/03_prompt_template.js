#!/usr/bin/env node
/**
 * 03_prompt_template.js - Compile prompt templates for dynamic AI interactions
 *
 * This example demonstrates advanced prompt engineering using templates:
 * 1. Load template files with variables and conditional logic
 * 2. Remove comment lines (///) for cleaner prompts
 * 3. Substitute variables like timestamps and user inputs
 * 4. Support basic conditional logic
 * 5. Generate dynamic, context-aware prompts
 *
 * Template features:
 * - Variables: [[.VarName]] replaced with actual values
 * - Comments: Lines starting with /// are removed
 * - Conditionals: Basic if/else based on category
 * - Timestamps: [[.Now]] replaced with current time
 *
 * Usage:
 *     node 03_prompt_template.js [category] [topic]
 *
 * Examples:
 *     node 03_prompt_template.js "Science" "Quantum Computing"
 *     node 03_prompt_template.js "History" "The Industrial Revolution"
 *     node 03_prompt_template.js "Technology" "Artificial Intelligence"
 *
 * Environment:
 *     GROQ_API_KEY - Your Groq API key (required)
 */

const https = require('https');
const fs = require('fs');
const path = require('path');

// Configuration
const API_HOST = 'api.groq.com';
const API_PATH = '/openai/v1/chat/completions';
const MODEL = 'meta-llama/llama-4-scout-17b-16e-instruct';

// Template configuration
const TEMPLATE_DIR = path.join(__dirname, '..', 'templates');
const TEMPLATE_FILE = path.join(TEMPLATE_DIR, 'essay_writer.txt');

// Pricing (per million tokens)
const INPUT_PRICE = 0.11;
const OUTPUT_PRICE = 0.34;

/**
 * Create an example template file if it doesn't exist
 */
function createExampleTemplate() {
    // Ensure template directory exists
    if (!fs.existsSync(TEMPLATE_DIR)) {
        fs.mkdirSync(TEMPLATE_DIR, { recursive: true });
    }

    const templateContent = `/// Template for writing educational essays
/// Variables: [[.Now]] = current timestamp, [[.Category]] = essay category, [[.Topic]] = essay topic
/// Comments starting with /// are removed during compilation

# 🎯 OBJECTIVE

Write an engaging, educational essay about [[.Topic]] in the [[.Category]] category.
The essay should be appropriate for students aged 14-18, informative yet accessible.
Current timestamp: [[.Now]]

# 📝 ESSAY REQUIREMENTS

* **Length:** 500-750 words (approximately 5-7 paragraphs)
* **Structure:** Introduction, 3-4 body paragraphs, conclusion
* **Tone:** Educational, engaging, age-appropriate
* **Citations:** Include at least 3 factual references

# 📚 CATEGORY: [[.Category]]

[[if .Category == "History"]]
Focus on:
- Cause and effect relationships
- Specific dates and key figures
- Historical significance and modern relevance
[[else if .Category == "Science"]]
Focus on:
- Fundamental concepts explained simply
- Real-world applications
- Scientific method and evidence
[[else if .Category == "Technology"]]
Focus on:
- Technical concepts in simple terms
- Benefits and challenges
- Future implications
[[else]]
Focus on:
- Comprehensive overview
- Relevant examples
- Balanced perspective
[[end]]

# OUTPUT FORMAT

Provide the essay with:
- Clear, descriptive title
- Well-structured paragraphs
- Key terms defined
- 2-3 discussion questions at the end`;

    fs.writeFileSync(TEMPLATE_FILE, templateContent);
    console.log(`✅ Created template file: ${TEMPLATE_FILE}`);
}

/**
 * Template processor class for handling template compilation
 */
class TemplateProcessor {
    constructor(templatePath) {
        this.templatePath = templatePath;
        this.templateContent = this.loadTemplate();
    }

    /**
     * Load template from file
     * @returns {string} Template content
     */
    loadTemplate() {
        if (!fs.existsSync(this.templatePath)) {
            throw new Error(`Template not found: ${this.templatePath}`);
        }
        return fs.readFileSync(this.templatePath, 'utf-8');
    }

    /**
     * Process template with variable substitution
     * @param {Object} variables - Variables to substitute
     * @returns {string} Processed template
     */
    process(variables) {
        let processed = this.templateContent;

        // Step 1: Remove comment lines (///)
        processed = this.removeComments(processed);

        // Step 2: Substitute variables
        processed = this.substituteVariables(processed, variables);

        // Step 3: Process conditionals
        processed = this.processConditionals(processed, variables);

        // Step 4: Clean up any remaining markers
        processed = this.cleanupMarkers(processed);

        return processed;
    }

    /**
     * Remove lines starting with ///
     * @param {string} text - Template text
     * @returns {string} Text with comments removed
     */
    removeComments(text) {
        const lines = text.split('\n');
        const filteredLines = lines.filter(line => !line.trim().startsWith('///'));
        return filteredLines.join('\n');
    }

    /**
     * Substitute [[.VarName]] with actual values
     * @param {string} text - Template text
     * @param {Object} variables - Variable dictionary
     * @returns {string} Text with variables substituted
     */
    substituteVariables(text, variables) {
        for (const [key, value] of Object.entries(variables)) {
            // Skip Topic variable as it will be in user message
            if (key === 'Topic') continue;
            const pattern = new RegExp(`\\[\\[\\.${key}\\]\\]`, 'g');
            text = text.replace(pattern, String(value));
        }
        return text;
    }

    /**
     * Process basic conditional blocks
     * @param {string} text - Template text
     * @param {Object} variables - Variable dictionary
     * @returns {string} Text with conditionals processed
     */
    processConditionals(text, variables) {
        const category = variables.Category || '';

        // Pattern to match conditional blocks
        const pattern = /\[\[if \.Category == "(.*?)"\]\]([\s\S]*?)\[\[end\]\]/g;

        text = text.replace(pattern, (match, conditionValue, content) => {
            // Check if condition matches
            if (conditionValue === category) {
                // Remove else blocks and return content
                content = content.replace(/\[\[else.*?\]\][\s\S]*?(?=\[\[end\]\]|$)/g, '');
                return content.trim();
            } else {
                // Look for matching else if
                const elseIfPattern = new RegExp(
                    `\\[\\[else if \\.Category == "${category}"\\]\\]([\\s\\S]*?)(?=\\[\\[else|\\[\\[end\\]\\])`,
                    'g'
                );
                const elseIfMatch = elseIfPattern.exec(content);
                if (elseIfMatch) {
                    return elseIfMatch[1].trim();
                }

                // Look for generic else
                const elsePattern = /\[\[else\]\]([\s\S]*?)(?=\[\[end\]\]|$)/;
                const elseMatch = elsePattern.exec(content);
                if (elseMatch) {
                    return elseMatch[1].trim();
                }
            }
            return '';
        });

        return text;
    }

    /**
     * Remove any remaining conditional markers
     * @param {string} text - Template text
     * @returns {string} Cleaned text
     */
    cleanupMarkers(text) {
        // Remove any remaining conditional markers
        text = text.replace(/\[\[if.*?\]\]/g, '');
        text = text.replace(/\[\[else.*?\]\]/g, '');
        text = text.replace(/\[\[end\]\]/g, '');

        // Clean up multiple blank lines
        text = text.replace(/\n\n\n+/g, '\n\n');

        return text.trim();
    }
}

/**
 * Make API request to Groq
 * @param {string} systemPrompt - System message content
 * @param {string} userPrompt - User message content
 * @param {string} apiKey - API key for authentication
 * @returns {Promise<Object>} API response
 */
function makeApiRequest(systemPrompt, userPrompt, apiKey) {
    return new Promise((resolve, reject) => {
        // Prepare request data
        const requestData = {
            model: MODEL,
            messages: [
                { role: 'system', content: systemPrompt },
                { role: 'user', content: userPrompt }
            ],
            max_tokens: 2000,
            temperature: 0.7
        };

        const jsonData = JSON.stringify(requestData);

        // Set up request options
        const options = {
            hostname: API_HOST,
            port: 443,
            path: API_PATH,
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${apiKey}`,
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(jsonData)
            }
        };

        // Make the request
        const req = https.request(options, (res) => {
            let data = '';

            res.on('data', (chunk) => {
                data += chunk;
            });

            res.on('end', () => {
                try {
                    const response = JSON.parse(data);

                    // Check for API errors
                    if (response.error) {
                        reject(new Error(`API Error: ${response.error.message || 'Unknown error'}`));
                    } else {
                        resolve(response);
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
        req.write(jsonData);
        req.end();
    });
}

/**
 * Display the API response
 * @param {Object} response - API response object
 */
function displayResponse(response) {
    // Extract content
    if (response.choices && response.choices.length > 0) {
        const content = response.choices[0].message.content;

        console.log('\n' + '='.repeat(70));
        console.log('GENERATED ESSAY');
        console.log('='.repeat(70));
        console.log(content);
    } else {
        console.log('No response generated');
    }

    // Display usage statistics
    if (response.usage) {
        const { prompt_tokens, completion_tokens, total_tokens } = response.usage;

        // Calculate costs
        const inputCost = (prompt_tokens * INPUT_PRICE) / 1_000_000;
        const outputCost = (completion_tokens * OUTPUT_PRICE) / 1_000_000;
        const totalCost = inputCost + outputCost;

        console.log('\n' + '='.repeat(70));
        console.log('USAGE STATISTICS');
        console.log('='.repeat(70));
        console.log(`Prompt tokens:     ${prompt_tokens.toLocaleString()}`);
        console.log(`Completion tokens: ${completion_tokens.toLocaleString()}`);
        console.log(`Total tokens:      ${total_tokens.toLocaleString()}`);
        console.log(`\nCost breakdown:`);
        console.log(`  Input:  $${inputCost.toFixed(6)}`);
        console.log(`  Output: $${outputCost.toFixed(6)}`);
        console.log(`  Total:  $${totalCost.toFixed(6)}`);
    }
}

/**
 * Main function to demonstrate template processing
 */
async function main() {
    // Parse command line arguments
    const category = process.argv[2] || 'Science';
    const topic = process.argv[3] || 'Climate Change';

    console.log('='.repeat(70));
    console.log('PROMPT TEMPLATE COMPILATION DEMO');
    console.log('='.repeat(70));
    console.log(`Category: ${category}`);
    console.log(`Topic: ${topic}`);

    // Check for API key
    const apiKey = process.env.GROQ_API_KEY;
    if (!apiKey) {
        console.log('\n❌ Error: GROQ_API_KEY environment variable not set');
        console.log("Set it with: export GROQ_API_KEY='your-api-key-here'");
        process.exit(1);
    }

    // Create template if it doesn't exist
    if (!fs.existsSync(TEMPLATE_FILE)) {
        console.log(`\n⚠️ Template not found at ${TEMPLATE_FILE}`);
        console.log('Creating example template...');
        createExampleTemplate();
    }

    // Get current timestamp
    const now = new Date().toISOString().replace('T', ' ').split('.')[0] + ' UTC';
    console.log(`Timestamp: ${now}`);

    // Prepare variables for template
    const variables = {
        Category: category,
        Topic: topic,
        Now: now
    };

    try {
        // Process template
        console.log('\n' + '='.repeat(70));
        console.log('PROCESSING TEMPLATE');
        console.log('='.repeat(70));
        console.log(`Loading template from: ${TEMPLATE_FILE}`);
        console.log('Processing steps:');
        console.log('  1. Removing comment lines (///)');
        console.log('  2. Substituting variables');
        console.log('  3. Processing conditionals');
        console.log('  4. Cleaning up markers');

        const processor = new TemplateProcessor(TEMPLATE_FILE);
        const processedPrompt = processor.process(variables);

        // Show preview of processed prompt
        console.log('\n' + '='.repeat(70));
        console.log('COMPILED PROMPT (PREVIEW)');
        console.log('='.repeat(70));
        const previewLength = Math.min(500, processedPrompt.length);
        console.log(processedPrompt.substring(0, previewLength) + '...');
        console.log(`\n(Showing first ${previewLength} characters of ${processedPrompt.length} total)`);

        // Create user message
        const userMessage = `Please write the essay about ${topic} as specified in the instructions.`;

        // Make API request
        console.log('\n' + '='.repeat(70));
        console.log('SENDING TO AI');
        console.log('='.repeat(70));
        console.log(`Model: ${MODEL}`);
        console.log('Making API request...');

        const response = await makeApiRequest(processedPrompt, userMessage, apiKey);

        // Display response
        displayResponse(response);

        // Educational notes
        console.log('\n' + '='.repeat(70));
        console.log('💡 EDUCATIONAL NOTES');
        console.log('='.repeat(70));
        console.log(`
Template-based prompting offers several advantages:

1. **Consistency**: Ensures uniform output format across requests
2. **Maintainability**: Templates can be updated without changing code
3. **Reusability**: Same template works for different inputs
4. **Clarity**: Separates prompt logic from application logic
5. **Version Control**: Templates can be tracked and versioned

Key techniques demonstrated:
- Variable substitution ([[.VarName]])
- Comment stripping (/// lines)
- Conditional logic (if/else blocks)
- Timestamp injection for context
- Category-specific instructions

Node.js-specific features used:
- Built-in fs module for file operations
- Built-in https module for API requests
- Regular expressions for pattern matching
- Template string literals for formatting
- Async/await for handling promises
        `);

    } catch (error) {
        console.error(`\n❌ Error: ${error.message}`);
        if (error.stack) {
            console.error(error.stack);
        }
        process.exit(1);
    }
}

// Run the main function
if (require.main === module) {
    main().catch(console.error);
}