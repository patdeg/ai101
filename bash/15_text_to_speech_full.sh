#!/bin/bash
################################################################################
# Example 15: Text-to-Speech with Fish Audio (via OpenRouter) (Full Version)
################################################################################
#
# This example demonstrates converting text to spoken audio using OpenRouter's
# /audio/speech endpoint and Fish Audio's S2.1 Pro Free model - a genuinely
# free TTS model, no card required.
#
# What you'll learn:
# - How to call a TTS model through OpenRouter's unified API
# - Optional voice selection via a Fish Audio reference ID
# - Audio format handling
#
# Prerequisites:
# - OPENROUTER_API_KEY environment variable
# - (optional) FISH_VOICE_1 - a Fish Audio voice reference ID, free at
#   https://fish.audio (sign up, browse the voice library, copy an ID)
# - Audio player (mpv, afplay, or similar)
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
if [ -z "$OPENROUTER_API_KEY" ]; then
  echo -e "${RED}Error: OPENROUTER_API_KEY not set${NC}"
  echo "Get your key from: https://openrouter.ai/keys"
  exit 1
fi

API_URL="https://openrouter.ai/api/v1/audio/speech"
MODEL="fish-audio/s2.1-pro-free:free"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Text-to-Speech with Fish Audio (OpenRouter)${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

################################################################################
# Demo 1: Basic TTS with the default voice
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 1: Basic Text-to-Speech${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

TEXT="Hello! This is an example of text-to-speech synthesis using Fish Audio's free TTS model."
OUTPUT_FILE="demo1_default_voice.mp3"

echo -e "${YELLOW}Text:${NC} $TEXT"
echo -e "${YELLOW}Voice:${NC} default (no referenceId supplied)"
echo -e "${YELLOW}Format:${NC} mp3"
echo ""

curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -d @- <<EOF > "$OUTPUT_FILE"
{
  "model": "$MODEL",
  "input": "$TEXT",
  "response_format": "mp3"
}
EOF

if [ -f "$OUTPUT_FILE" ]; then
  FILE_SIZE=$(wc -c < "$OUTPUT_FILE")
  echo -e "${GREEN}✓ Audio created: $OUTPUT_FILE ($FILE_SIZE bytes)${NC}"
  echo ""
fi

################################################################################
# Demo 2: A specific voice, if the learner has picked one
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 2: A Voice Picked From Fish Audio${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

if [ -n "$FISH_VOICE_1" ]; then
  COMPARISON_TEXT="This is the same text, spoken with a voice picked from Fish Audio's library."
  VOICE_OUTPUT_FILE="demo2_chosen_voice.mp3"

  curl -s -X POST "$API_URL" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $OPENROUTER_API_KEY" \
    -d @- <<EOF > "$VOICE_OUTPUT_FILE"
{
  "model": "$MODEL",
  "input": "$COMPARISON_TEXT",
  "providerOptions": { "fishAudio": { "referenceId": "$FISH_VOICE_1" } },
  "response_format": "mp3"
}
EOF

  if [ -f "$VOICE_OUTPUT_FILE" ]; then
    FILE_SIZE=$(wc -c < "$VOICE_OUTPUT_FILE")
    echo -e "${GREEN}✓ Chosen voice: $VOICE_OUTPUT_FILE ($FILE_SIZE bytes)${NC}"
  fi
  echo ""
else
  echo -e "${YELLOW}Tip:${NC} set FISH_VOICE_1 to a voice ID from https://fish.audio to"
  echo "hear a specific voice instead of the default."
  echo ""
fi

################################################################################
# Cost Analysis
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Cost Analysis${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

echo -e "${YELLOW}Pricing ($MODEL):${NC}"
echo "  \$0 - free tier, rate-limited, best-effort availability"
echo "  (meant for learning/prototyping, not production)"
echo ""

################################################################################
# Summary
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "✓ Generated audio with the default voice"
echo "✓ Showed how to pick a specific voice by reference ID"
echo "✓ Zero cost on the free tier"
echo ""

echo -e "${YELLOW}To play audio files:${NC}"
echo "  mpv demo1_default_voice.mp3"
echo "  afplay demo1_default_voice.mp3  (macOS)"
echo ""

echo -e "${YELLOW}Next:${NC} Example 16 (podcast) uses this same model for a"
echo "two-speaker conversation in a single API call."
echo ""
