#!/bin/sh
################################################################################
# Example 16: Multi-Speaker Podcast Generation (Minimal Version)
################################################################################
#
# Generate a podcast with two speakers in a SINGLE API call using OpenRouter's
# audio/speech endpoint and the free Fish Audio S2.1 Pro model.
#
# Prerequisites:
# - OPENROUTER_API_KEY environment variable
# - FISH_VOICE_1 / FISH_VOICE_2 environment variables — two Fish Audio voice
#   reference IDs. Get free ones by creating an account at https://fish.audio,
#   browsing the voice library, and copying each voice's reference ID.
#
# Usage:
#   export OPENROUTER_API_KEY="sk-or-your-api-key"
#   export FISH_VOICE_1="your-first-voice-reference-id"
#   export FISH_VOICE_2="your-second-voice-reference-id"
#   ./16_podcast_minimal.sh
#
################################################################################

# Check for required environment variables
if [ -z "$OPENROUTER_API_KEY" ]; then
  echo "Error: OPENROUTER_API_KEY not set"
  echo "Get your key from: https://openrouter.ai/keys"
  exit 1
fi
if [ -z "$FISH_VOICE_1" ] || [ -z "$FISH_VOICE_2" ]; then
  echo "Error: FISH_VOICE_1 and FISH_VOICE_2 must both be set"
  echo "Get two free voice reference IDs from https://fish.audio (sign up,"
  echo "browse the voice library, copy each voice's reference ID)"
  exit 1
fi

API_URL="https://openrouter.ai/api/v1/audio/speech"

# The podcast script - written as "Name: line" for readability
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

# Fish Audio's multi-speaker format doesn't use names — it uses index tags
# <|speaker:0|> / <|speaker:1|> matched against the referenceId array below.
# Convert "Alex: ..." -> "<|speaker:0|>..." and "Sam: ..." -> "<|speaker:1|>..."
TAGGED_SCRIPT=$(echo "$SCRIPT" | sed -e 's/^Alex: /<|speaker:0|>/' -e 's/^Sam: /<|speaker:1|>/' | tr '\n' ' ')

echo "Generating podcast with two speakers..."
echo "Script length: $(echo "$SCRIPT" | wc -c) characters"

# Single API call generates audio with both voices
# - referenceId[0] (Alex) is spoken by <|speaker:0|> turns
# - referenceId[1] (Sam) is spoken by <|speaker:1|> turns
# API returns binary audio directly (this is the free ":free" tier — $0.00)
curl -s -X POST "$API_URL" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -H "Content-Type: application/json" \
  -o columbus_podcast.mp3 \
  -d @- <<EOF
{
  "model": "fish-audio/s2.1-pro-free:free",
  "input": "$TAGGED_SCRIPT",
  "providerOptions": {
    "fishAudio": { "referenceId": ["$FISH_VOICE_1", "$FISH_VOICE_2"] }
  },
  "response_format": "mp3"
}
EOF

# Check if response is an error (JSON) or audio (binary)
if [ -f "columbus_podcast.mp3" ] && [ -s "columbus_podcast.mp3" ]; then
  FIRST_CHAR=$(head -c 1 columbus_podcast.mp3)
  if [ "$FIRST_CHAR" = "{" ]; then
    # Response is JSON error
    echo "API Error:"
    cat columbus_podcast.mp3
    echo ""
    rm -f columbus_podcast.mp3
    exit 1
  else
    echo "Created: columbus_podcast.mp3 ($(wc -c < columbus_podcast.mp3) bytes)"
    echo "Cost: \$0.00 (fish-audio/s2.1-pro-free:free tier)"
    echo "Play with: mpv columbus_podcast.mp3"
  fi
else
  echo "Error: No response from API"
  exit 1
fi
