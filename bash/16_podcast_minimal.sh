#!/bin/sh
################################################################################
# Example 16: Multi-Speaker Podcast Generation (Minimal Version)
################################################################################
#
# Generate a podcast with two speakers in a SINGLE API call using Demeterics.
#
# Prerequisites:
# - DEMETERICS_API_KEY environment variable
#
# Usage:
#   export DEMETERICS_API_KEY="dmt_your-api-key"
#   ./16_podcast_minimal.sh
#
################################################################################

# Check for API key
if [ -z "$DEMETERICS_API_KEY" ]; then
  echo "Error: DEMETERICS_API_KEY not set"
  echo "Get your key from: https://demeterics.com"
  exit 1
fi

API_URL="https://api.demeterics.com/tts/v1/generate"

# The podcast script - Speaker names must match speakers array exactly
SCRIPT='Alex: Hey, welcome back to The Deep Dive! I am Alex.
Sam: And I am Sam. Today we are diving into something that every kid learns in school, but honestly, gets way more complicated when you dig into it.
Alex: Columbus. Christopher Columbus. 1492. In fourteen hundred ninety two, Columbus sailed the ocean blue.
Sam: Right! But here is the thing. That rhyme does not tell you much, does it?
Alex: Not at all! So get this. Columbus was not even trying to discover America. He thought he was finding a shortcut to Asia.
Sam: To India, specifically. Which is why he called the native people Indians.
Alex: Exactly! A massive geography fail that stuck around for five hundred years.
Sam: So let us set the scene. It is 1492. Columbus is Italian, from Genoa, but he is sailing for Spain.
Alex: Because Portugal said no! He pitched this idea everywhere. England said no. France said no. Portugal said hard no.
Sam: But Queen Isabella and King Ferdinand of Spain said, you know what, sure, let us do it.
Alex: And here is what is wild. Columbus was wrong about basically everything. He thought the Earth was way smaller than it actually is.
Sam: Most educated people knew the Earth was round. That is a myth that he proved it. They knew. They just thought his math was bad.
Alex: Because it was! If America was not there, he and his crew would have starved in the middle of the ocean.
Sam: So he gets lucky. Three ships. The Nina, the Pinta, and the Santa Maria. About two months at sea.
Alex: And on October 12th, 1492, they land in the Bahamas. Not mainland America. The Bahamas.
Sam: An island he named San Salvador. And the people already living there? The Taino people. They had been there for centuries.
Alex: So this whole discovery narrative is complicated, to say the least.
Sam: Very complicated. It is really the story of a European arriving somewhere that was not empty. And that changes everything.
Alex: That is the deep dive for today. Thanks for listening, everyone!
Sam: See you next time!'

echo "Generating podcast with two speakers..."
echo "Script length: $(echo "$SCRIPT" | wc -c) characters"

# Single API call generates audio with both voices (Deep Dive style)
# - Alex uses Charon voice (informative, clear - explains concepts)
# - Sam uses Puck voice (upbeat - energetic, enthusiastic)
# API returns binary audio directly with metadata in headers
HEADERS_FILE=$(mktemp)
curl -s -X POST "$API_URL" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -H "Content-Type: application/json" \
  -o columbus_podcast.wav \
  -D "$HEADERS_FILE" \
  -d @- <<EOF
{
  "provider": "gemini",
  "input": "$SCRIPT",
  "speakers": [
    {"name": "Alex", "voice": "Charon"},
    {"name": "Sam", "voice": "Puck"}
  ]
}
EOF

# Check if response is an error (JSON) or audio (binary)
if [ -f "columbus_podcast.wav" ] && [ -s "columbus_podcast.wav" ]; then
  FIRST_CHAR=$(head -c 1 columbus_podcast.wav)
  if [ "$FIRST_CHAR" = "{" ]; then
    # Response is JSON error
    echo "API Error:"
    cat columbus_podcast.wav
    echo ""
    rm -f columbus_podcast.wav "$HEADERS_FILE"
    exit 1
  else
    # Success - extract metadata from headers
    COST=$(grep -i "X-Cost-USD" "$HEADERS_FILE" | cut -d' ' -f2 | tr -d '\r')
    DURATION=$(grep -i "X-Duration-Seconds" "$HEADERS_FILE" | cut -d' ' -f2 | tr -d '\r')
    rm -f "$HEADERS_FILE"
    echo "Duration: ${DURATION}s, Cost: \$${COST}"
    echo "Created: columbus_podcast.wav ($(wc -c < columbus_podcast.wav) bytes)"
    echo "Play with: mpv columbus_podcast.wav"
  fi
else
  echo "Error: No response from API"
  rm -f "$HEADERS_FILE"
  exit 1
fi
