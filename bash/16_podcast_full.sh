#!/bin/bash
################################################################################
# Example 16: Multi-Speaker Podcast Generation (Full Version)
################################################################################
#
# This example demonstrates how to generate a podcast with multiple speakers
# using OpenRouter's audio/speech endpoint and Fish Audio's S2.1 Pro model,
# in a SINGLE API call.
#
# What you'll learn:
# - Multi-speaker audio generation with distinct voices
# - NPR/NotebookLM "Deep Dive" podcast style
# - Fish Audio's <|speaker:N|> tagging convention for turn-taking
# - Script formatting with "Speaker: text" pattern, then converting it
# - Audio output handling (MP3 format)
#
# Key Feature: Single API call generates all speakers - no audio stitching!
#
# Prerequisites:
# - OPENROUTER_API_KEY environment variable
# - FISH_VOICE_1 / FISH_VOICE_2 environment variables — two Fish Audio voice
#   reference IDs, free at https://fish.audio (sign up, browse the voice
#   library, copy each voice's reference ID)
# - jq for JSON escaping
#
# API Limitation: Fish Audio S2-Pro supports a maximum of 2 speakers per
# request, matched by index against the referenceId array.
#
################################################################################

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

# Check for required environment variables
if [ -z "$OPENROUTER_API_KEY" ]; then
  echo -e "${RED}Error: OPENROUTER_API_KEY not set${NC}"
  echo "Get your key from: https://openrouter.ai/keys"
  exit 1
fi
if [ -z "$FISH_VOICE_1" ] || [ -z "$FISH_VOICE_2" ]; then
  echo -e "${RED}Error: FISH_VOICE_1 and FISH_VOICE_2 must both be set${NC}"
  echo "Get two free voice reference IDs from https://fish.audio (sign up,"
  echo "browse the voice library, copy each voice's reference ID)"
  exit 1
fi

API_URL="https://openrouter.ai/api/v1/audio/speech"
MODEL="fish-audio/s2.1-pro-free:free"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Multi-Speaker Podcast Generation${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

################################################################################
# Podcast Configuration
################################################################################

# Speaker index 0 and index 1 are matched to referenceId[0] / referenceId[1]
# below — Fish Audio identifies speakers by position, not by name.
SPEAKER1_NAME="Alex"
SPEAKER2_NAME="Sam"

echo -e "${YELLOW}Podcast Configuration:${NC}"
echo "  Host 1: $SPEAKER1_NAME  -> <|speaker:0|> -> voice \$FISH_VOICE_1"
echo "  Host 2: $SPEAKER2_NAME  -> <|speaker:1|> -> voice \$FISH_VOICE_2"
echo ""

################################################################################
# The Podcast Script
################################################################################
# Format: "SpeakerName: dialogue text"
# - Speaker names are just for readability here — they get converted to
#   <|speaker:0|> / <|speaker:1|> tags below before the API call
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
# Convert to Fish Audio's <|speaker:N|> tagging convention
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Converting to Fish Audio speaker tags${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

TAGGED_SCRIPT=$(echo "$SCRIPT" \
  | sed -e "s/^${SPEAKER1_NAME}: /<|speaker:0|>/" -e "s/^${SPEAKER2_NAME}: /<|speaker:1|>/" \
  | tr '\n' ' ')

echo "  Alex: ...   -> <|speaker:0|>...    (voice: \$FISH_VOICE_1)"
echo "  Sam: ...    -> <|speaker:1|>...    (voice: \$FISH_VOICE_2)"
echo ""

################################################################################
# Generate Podcast Audio
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Generating Podcast Audio${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

OUTPUT_FILE="columbus_podcast.mp3"

echo -e "${YELLOW}Making API request...${NC}"
echo "  Endpoint: $API_URL"
echo "  Model: $MODEL"
echo "  Output: $OUTPUT_FILE (MP3 format)"
echo ""

# Escape the tagged script for JSON (jq -Rs turns raw text into a quoted,
# escaped JSON string literal)
SCRIPT_ESCAPED=$(printf '%s' "$TAGGED_SCRIPT" | jq -Rs .)

# Build the referenceId array as JSON
REFERENCE_IDS=$(jq -n --arg v1 "$FISH_VOICE_1" --arg v2 "$FISH_VOICE_2" '[$v1, $v2]')

# Make the API request - OpenRouter/Fish Audio returns audio bytes directly
HTTP_CODE=$(curl -s -w "%{http_code}" -o "$OUTPUT_FILE" -X POST "$API_URL" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -H "Content-Type: application/json" \
  -d @- <<EOF
{
  "model": "$MODEL",
  "input": $SCRIPT_ESCAPED,
  "providerOptions": {
    "fishAudio": { "referenceId": $REFERENCE_IDS }
  },
  "response_format": "mp3"
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
  echo "  Cost: \$0.00 (fish-audio/s2.1-pro-free:free tier)"
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
echo "  aplay $OUTPUT_FILE        # Linux (via ffmpeg/mpg123 plugin)"
echo "  afplay $OUTPUT_FILE       # macOS"
echo "  vlc $OUTPUT_FILE          # Cross-platform"
echo ""

################################################################################
# Voice Reference
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Finding Fish Audio Voices${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "${YELLOW}Fish Audio voices are identified by opaque reference IDs, not names:${NC}"
echo "  1. Create a free account at https://fish.audio"
echo "  2. Browse the voice library (many free community voices)"
echo "  3. Copy a voice's reference ID from its page"
echo "  4. Set FISH_VOICE_1 and FISH_VOICE_2 to two different IDs"
echo ""
echo -e "${YELLOW}Tips for picking a pairing:${NC}"
echo "  - Pick voices with different pitch/energy so listeners can tell"
echo "    speakers apart without needing names announced"
echo "  - Preview voices in Fish Audio's playground before committing"
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
