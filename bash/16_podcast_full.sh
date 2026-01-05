#!/bin/bash
################################################################################
# Example 16: Multi-Speaker Podcast Generation (Full Version)
################################################################################
#
# This example demonstrates how to generate a podcast with multiple speakers
# using Demeterics multi-speaker TTS API in a SINGLE API call.
#
# What you'll learn:
# - Multi-speaker audio generation with distinct voices
# - NPR/NotebookLM "Deep Dive" podcast style
# - Voice selection from 30 available options
# - Script formatting with "Speaker: text" pattern
# - Audio output handling (WAV format)
#
# Key Feature: Single API call generates all speakers - no audio stitching!
#
# Prerequisites:
# - DEMETERICS_API_KEY environment variable
# - jq for JSON parsing (optional, for pretty output)
#
# Available Voices (30 total):
#   Puck (upbeat), Kore (firm), Charon (informative), Zephyr (bright),
#   Fenrir (excitable), Leda (youthful), Aoede (breezy), Sulafat (warm),
#   Achird (friendly), and 21 others.
#
# API Limitation: Maximum 2 speakers per request
#
################################################################################

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

# Check for API key
if [ -z "$DEMETERICS_API_KEY" ]; then
  echo -e "${RED}Error: DEMETERICS_API_KEY not set${NC}"
  echo "Get your key from: https://demeterics.com"
  exit 1
fi

API_URL="https://api.demeterics.com/tts/v1/generate"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Multi-Speaker Podcast Generation${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

################################################################################
# Podcast Configuration
################################################################################

# Voice selection - Deep Dive podcast style pairing
SPEAKER1_NAME="Alex"
SPEAKER1_VOICE="Charon"   # Informative, clear - explains concepts
SPEAKER2_NAME="Sam"
SPEAKER2_VOICE="Puck"     # Upbeat - energetic, enthusiastic

echo -e "${YELLOW}Podcast Configuration:${NC}"
echo "  Host 1: $SPEAKER1_NAME (Voice: $SPEAKER1_VOICE)"
echo "  Host 2: $SPEAKER2_NAME (Voice: $SPEAKER2_VOICE)"
echo ""

################################################################################
# The Podcast Script
################################################################################
# Format: "SpeakerName: dialogue text"
# - Speaker names must match exactly in speakers array
# - Avoid special characters that might break JSON (use "is" not "it's")
# - Each line is a speaking turn
################################################################################

read -r -d '' SCRIPT << 'ENDSCRIPT'
Alex: Hey, welcome back to The Deep Dive! I am Alex.
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
Sam: See you next time!
ENDSCRIPT

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Podcast Script${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo "$SCRIPT"
echo ""

# Script statistics
CHAR_COUNT=${#SCRIPT}
LINE_COUNT=$(echo "$SCRIPT" | wc -l)
ALEX_LINES=$(echo "$SCRIPT" | grep -c "^Alex:")
SAM_LINES=$(echo "$SCRIPT" | grep -c "^Sam:")

echo -e "${YELLOW}Script Statistics:${NC}"
echo "  Total characters: $CHAR_COUNT"
echo "  Total lines: $LINE_COUNT"
echo "  $SPEAKER1_NAME lines: $ALEX_LINES"
echo "  $SPEAKER2_NAME lines: $SAM_LINES"
echo ""

################################################################################
# Generate Podcast Audio
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Generating Podcast Audio${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

OUTPUT_FILE="columbus_podcast.wav"

echo -e "${YELLOW}Making API request...${NC}"
echo "  Endpoint: $API_URL"
echo "  Provider: gemini"
echo "  Output: $OUTPUT_FILE (WAV format)"
echo ""

# Escape the script for JSON (newlines become \n)
SCRIPT_ESCAPED=$(echo "$SCRIPT" | jq -Rs . 2>/dev/null || echo "\"$SCRIPT\"" | sed 's/$/\\n/' | tr -d '\n')

# Make the API request - Demeterics returns audio directly
HTTP_CODE=$(curl -s -w "%{http_code}" -o "$OUTPUT_FILE" -X POST "$API_URL" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<EOF
{
  "provider": "gemini",
  "input": $SCRIPT_ESCAPED,
  "speakers": [
    {"name": "$SPEAKER1_NAME", "voice": "$SPEAKER1_VOICE"},
    {"name": "$SPEAKER2_NAME", "voice": "$SPEAKER2_VOICE"}
  ]
}
EOF
)

# Check HTTP status
if [ "$HTTP_CODE" != "200" ]; then
  echo -e "${RED}API Error: HTTP $HTTP_CODE${NC}"
  if [ -f "$OUTPUT_FILE" ]; then
    cat "$OUTPUT_FILE"
    rm "$OUTPUT_FILE"
  fi
  exit 1
fi

if [ -f "$OUTPUT_FILE" ] && [ -s "$OUTPUT_FILE" ]; then
  FILE_SIZE=$(wc -c < "$OUTPUT_FILE")
  FILE_SIZE_KB=$((FILE_SIZE / 1024))

  echo -e "${GREEN}Success!${NC}"
  echo ""
  echo -e "${YELLOW}Output:${NC}"
  echo "  File: $OUTPUT_FILE"
  echo "  Size: $FILE_SIZE bytes ($FILE_SIZE_KB KB)"
  echo ""
else
  echo -e "${RED}Error: Failed to create audio file${NC}"
  exit 1
fi

################################################################################
# Playback Instructions
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Playback${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "${YELLOW}To play the podcast:${NC}"
echo "  mpv $OUTPUT_FILE"
echo "  aplay $OUTPUT_FILE        # Linux"
echo "  afplay $OUTPUT_FILE       # macOS"
echo "  vlc $OUTPUT_FILE          # Cross-platform"
echo ""

################################################################################
# Voice Reference
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Available Voices (30 total)${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "${YELLOW}Popular voices:${NC}"
echo "  Puck     - Upbeat, energetic"
echo "  Kore     - Firm, confident"
echo "  Charon   - Informative, clear"
echo "  Zephyr   - Bright, cheerful"
echo "  Fenrir   - Excitable, dynamic"
echo "  Leda     - Youthful, fresh"
echo "  Aoede    - Breezy, warm"
echo "  Sulafat  - Warm, friendly"
echo "  Achird   - Friendly, approachable"
echo ""
echo -e "${YELLOW}Recommended pairings for podcasts:${NC}"
echo "  Charon + Puck    (informative + upbeat - Deep Dive style)"
echo "  Aoede + Charon   (warm female + informative male)"
echo "  Kore + Puck      (firm female + upbeat male)"
echo ""

################################################################################
# Tips
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Tips for Great Podcasts${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "1. Use conversational language (contractions, casual phrasing)"
echo "2. Add vocal cues: 'get this', 'here is the thing', 'right?'"
echo "3. Include natural reactions: 'Exactly!', 'Right!', 'Wow!'"
echo "4. Balance speaking time between hosts"
echo "5. Keep individual turns short (1-3 sentences)"
echo "6. End with clear sign-off from both hosts"
echo ""
echo -e "${GREEN}Podcast generation complete!${NC}"
