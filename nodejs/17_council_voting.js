#!/usr/bin/env node
/**
 * Exercise 17: Council Voting - Multi-Persona Content Selection
 *
 * Demonstrates:
 * - Demeterics Council API for content evaluation
 * - Multi-persona voting to select the best option
 * - Vote tallying and consensus metrics
 */

const https = require('https');

// Check for API keys
const DEMETERICS_KEY = process.env.DEMETERICS_API_KEY;
const GROQ_KEY = process.env.GROQ_API_KEY;

if (!DEMETERICS_KEY || !GROQ_KEY) {
    console.error('Error: Both DEMETERICS_API_KEY and GROQ_API_KEY must be set');
    console.error('Get your Demeterics key: https://demeterics.ai');
    console.error('Get your Groq key: https://console.groq.com');
    process.exit(1);
}

// Stories from December 3rd in History (simplified)
const STORIES = `A) John Paul Jones raises the first American flag on a warship in 1775.
Cannons fire over Boston Harbor as sailors cheer. The Grand Union Flag flies
for the first time, marking the birth of the American naval tradition.

B) Dr. Christiaan Barnard performs the first human heart transplant in 1967.
In a tense operating room at Groote Schuur Hospital, surgeons lift a donor
heart. When it begins to beat in the patient's chest, medical history is made.

C) Astronomer Charles Perrine discovers Jupiter's moon Himalia in 1904.
Peering through the telescope at Lick Observatory, a faint speck moves against
Jupiter's clouds. Humanity's map of the solar system expands once more.

D) Edith Sampson becomes the first African-American female judge in 1962.
The gavel slams in a packed Chicago courtroom. Cameras flash as Sampson takes
the bench, a watershed moment for American justice and civil rights.`;

/**
 * Call the Demeterics Council API for voting evaluation
 */
function callCouncil(question, content, numPersonas = 8) {
    return new Promise((resolve, reject) => {
        // Use dual-key format: demeterics_key;groq_key
        const authHeader = `${DEMETERICS_KEY};${GROQ_KEY}`;

        const body = JSON.stringify({
            question,
            content,
            num_personas: numPersonas
        });

        const options = {
            hostname: 'api.demeterics.com',
            port: 443,
            path: '/council/v1/evaluate',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${authHeader}`,
                'Content-Length': Buffer.byteLength(body)
            }
        };

        const req = https.request(options, (res) => {
            let data = '';
            res.on('data', chunk => data += chunk);
            res.on('end', () => {
                try {
                    resolve(JSON.parse(data));
                } catch (e) {
                    reject(new Error(`Failed to parse response: ${data}`));
                }
            });
        });

        req.on('error', reject);
        req.write(body);
        req.end();
    });
}

/**
 * Display voting results in a readable format
 */
function displayResults(result) {
    console.log('='.repeat(60));
    console.log('COUNCIL VOTING RESULTS');
    console.log('='.repeat(60));
    console.log();

    // Check for errors
    if (result.error) {
        console.log(`Error: ${result.error.message || 'Unknown error'}`);
        return;
    }

    const stats = result.stats || {};

    // Winner and consensus
    console.log(`Winner: ${stats.majority_vote || 'N/A'}`);
    console.log(`Consensus: ${stats.vote_consensus || 'N/A'}`);
    console.log();

    // Vote breakdown
    console.log('Vote Breakdown:');
    const voteBreakdown = stats.vote_breakdown || {};
    const sortedVotes = Object.entries(voteBreakdown).sort((a, b) => b[1] - a[1]);
    for (const [option, count] of sortedVotes) {
        const bar = '*'.repeat(count);
        console.log(`  ${option}: ${bar} (${count} votes)`);
    }
    console.log();

    // Overall score
    console.log(`Average Interest Level: ${(stats.average_interested_level || 0).toFixed(1)}/100`);
    console.log(`Personas: ${stats.successful_responses || 0}/${stats.total_personas || 0} responded`);
    console.log();

    // Individual votes
    console.log('Individual Votes:');
    console.log('-'.repeat(60));
    for (const persona of result.persona_responses || []) {
        console.log(`  ${persona.name || 'Unknown'}: ${persona.vote || 'N/A'}`);
        console.log(`    Reason: ${persona.vote_reason || 'No reason given'}`);
        console.log(`    Interest: ${persona.interested_level || 0}/100`);
        console.log();
    }

    // Summary
    console.log('='.repeat(60));
    console.log('COUNCIL SUMMARY');
    console.log('='.repeat(60));
    console.log(result.summary || 'No summary available');
    console.log();

    // Usage info
    const usage = result.usage || {};
    if (Object.keys(usage).length > 0) {
        console.log('='.repeat(60));
        console.log('USAGE');
        console.log('='.repeat(60));
        console.log(`Total API calls: ${usage.total_call_count || 0}`);
        console.log(`Total tokens: ${usage.total_tokens || 0}`);
        console.log(`Cost: $${(usage.total_cost_usd || 0).toFixed(4)}`);
    }
}

async function main() {
    console.log('='.repeat(60));
    console.log('Council Voting Demo - Best Story Selection');
    console.log('='.repeat(60));
    console.log();
    console.log('Question: Which story would you most want to watch as a short video?');
    console.log();
    console.log('Stories:');
    console.log(STORIES);
    console.log();
    console.log('Sending to Council API with 8 personas...');
    console.log();

    try {
        const result = await callCouncil(
            'Which story would you most want to watch as a short video?',
            STORIES,
            8
        );
        displayResults(result);
    } catch (error) {
        console.error(`Error: ${error.message}`);
        process.exit(1);
    }
}

main();
