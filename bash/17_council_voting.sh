#!/bin/bash
# Exercise 17: Council Voting - Multi-Persona Content Selection
#
# Demonstrates:
# - Demeterics Council API for content evaluation
# - Multi-persona voting to select the best option
# - Vote tallying and consensus metrics

set -e

# Check for API keys
if [ -z "$DEMETERICS_API_KEY" ] || [ -z "$GROQ_API_KEY" ]; then
    echo "Error: Both DEMETERICS_API_KEY and GROQ_API_KEY must be set" >&2
    echo "Get your Demeterics key: https://demeterics.ai" >&2
    echo "Get your Groq key: https://console.groq.com" >&2
    exit 1
fi

# Use dual-key format: demeterics_key;groq_key
AUTH_HEADER="${DEMETERICS_API_KEY};${GROQ_API_KEY}"

# Stories from December 3rd in History (simplified)
STORIES='A) John Paul Jones raises the first American flag on a warship in 1775.
Cannons fire over Boston Harbor as sailors cheer. The Grand Union Flag flies
for the first time, marking the birth of the American naval tradition.

B) Dr. Christiaan Barnard performs the first human heart transplant in 1967.
In a tense operating room at Groote Schuur Hospital, surgeons lift a donor
heart. When it begins to beat in the patient chest, medical history is made.

C) Astronomer Charles Perrine discovers Jupiters moon Himalia in 1904.
Peering through the telescope at Lick Observatory, a faint speck moves against
Jupiters clouds. Humanitys map of the solar system expands once more.

D) Edith Sampson becomes the first African-American female judge in 1962.
The gavel slams in a packed Chicago courtroom. Cameras flash as Sampson takes
the bench, a watershed moment for American justice and civil rights.'

echo "============================================================"
echo "Council Voting Demo - Best Story Selection"
echo "============================================================"
echo ""
echo "Question: Which story would you most want to watch as a short video?"
echo ""
echo "Stories:"
echo "$STORIES"
echo ""
echo "Sending to Council API with 8 personas..."
echo ""

# Build JSON request (escape special characters)
REQUEST_JSON=$(jq -n \
    --arg question "Which story would you most want to watch as a short video?" \
    --arg content "$STORIES" \
    '{
        question: $question,
        content: $content,
        num_personas: 8
    }')

# Call the Council API
RESPONSE=$(curl -s -X POST "https://api.demeterics.com/council/v1/evaluate" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $AUTH_HEADER" \
    -d "$REQUEST_JSON")

# Check for curl errors
if [ $? -ne 0 ]; then
    echo "Error: Failed to call Council API" >&2
    exit 1
fi

# Check for API errors
ERROR=$(echo "$RESPONSE" | jq -r '.error.message // empty')
if [ -n "$ERROR" ]; then
    echo "API Error: $ERROR" >&2
    exit 1
fi

echo "============================================================"
echo "COUNCIL VOTING RESULTS"
echo "============================================================"
echo ""

# Extract and display results
WINNER=$(echo "$RESPONSE" | jq -r '.stats.majority_vote // "N/A"')
CONSENSUS=$(echo "$RESPONSE" | jq -r '.stats.vote_consensus // "N/A"')

echo "Winner: $WINNER"
echo "Consensus: $CONSENSUS"
echo ""

echo "Vote Breakdown:"
echo "$RESPONSE" | jq -r '.stats.vote_breakdown | to_entries | sort_by(-.value) | .[] | "  \(.key): \("*" * .value) (\(.value) votes)"'
echo ""

AVG_LEVEL=$(echo "$RESPONSE" | jq -r '.stats.average_interested_level // 0')
SUCCESSFUL=$(echo "$RESPONSE" | jq -r '.stats.successful_responses // 0')
TOTAL=$(echo "$RESPONSE" | jq -r '.stats.total_personas // 0')

echo "Average Interest Level: ${AVG_LEVEL}/100"
echo "Personas: ${SUCCESSFUL}/${TOTAL} responded"
echo ""

echo "Individual Votes:"
echo "------------------------------------------------------------"
echo "$RESPONSE" | jq -r '.persona_responses[] | "  \(.name): \(.vote // "N/A")\n    Reason: \(.vote_reason // "No reason given")\n    Interest: \(.interested_level // 0)/100\n"'

echo "============================================================"
echo "COUNCIL SUMMARY"
echo "============================================================"
echo "$RESPONSE" | jq -r '.summary // "No summary available"'
echo ""

echo "============================================================"
echo "USAGE"
echo "============================================================"
CALLS=$(echo "$RESPONSE" | jq -r '.usage.total_call_count // 0')
TOKENS=$(echo "$RESPONSE" | jq -r '.usage.total_tokens // 0')
COST=$(echo "$RESPONSE" | jq -r '.usage.total_cost_usd // 0')

echo "Total API calls: $CALLS"
echo "Total tokens: $TOKENS"
printf "Cost: \$%.4f\n" "$COST"
