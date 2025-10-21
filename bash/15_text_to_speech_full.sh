#!/bin/bash
################################################################################
# Example 14: Text-to-Speech with OpenAI (Full Version)
################################################################################
#
# This example demonstrates OpenAI's text-to-speech API with multiple voices,
# instructions for voice customization, speed control, and format options.
#
# What you'll learn:
# - How to use OpenAI's affordable TTS model
# - 11 different voice options
# - Instructions parameter for voice customization
# - Speed control (0.25x to 4.0x)
# - Multiple audio formats
# - Multi-language support
#
# Prerequisites:
# - OPENAI_API_KEY environment variable
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
if [ -z "$OPENAI_API_KEY" ]; then
  echo -e "${RED}Error: OPENAI_API_KEY not set${NC}"
  echo "Get your key from: https://platform.openai.com"
  exit 1
fi

API_URL="https://api.openai.com/v1/audio/speech"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Text-to-Speech with OpenAI${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

################################################################################
# Demo 1: Basic TTS
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 1: Basic Text-to-Speech${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

TEXT="Hello! This is an example of text-to-speech synthesis using OpenAI's affordable TTS model."
VOICE="alloy"
OUTPUT_FILE="demo1_alloy.mp3"

echo -e "${YELLOW}Text:${NC} $TEXT"
echo -e "${YELLOW}Voice:${NC} $VOICE"
echo -e "${YELLOW}Format:${NC} mp3"
echo ""

curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<EOF > "$OUTPUT_FILE"
{
  "model": "gpt-4o-mini-tts",
  "input": "$TEXT",
  "voice": "$VOICE",
  "response_format": "mp3"
}
EOF

if [ -f "$OUTPUT_FILE" ]; then
  FILE_SIZE=$(wc -c < "$OUTPUT_FILE")
  echo -e "${GREEN}✓ Audio created: $OUTPUT_FILE ($FILE_SIZE bytes)${NC}"
  echo ""
fi

################################################################################
# Demo 2: Voice Comparison
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 2: Comparing All 11 Voices${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

COMPARISON_TEXT="Welcome to OpenAI's text-to-speech demonstration."

# All 11 voices with characteristics
declare -a VOICES=(
  "alloy:Neutral, balanced"
  "ash:Clear, articulate"
  "ballad:Smooth, expressive"
  "coral:Warm, friendly"
  "echo:Clear, professional"
  "fable:Narrative, storytelling"
  "onyx:Deep, authoritative"
  "nova:Energetic, youthful"
  "sage:Calm, measured"
  "shimmer:Bright, cheerful"
  "verse:Poetic, expressive"
)

echo -e "${YELLOW}Generating samples with all 11 voices...${NC}"
echo ""

for VOICE_DESC in "${VOICES[@]}"; do
  VOICE="${VOICE_DESC%%:*}"
  DESC="${VOICE_DESC#*:}"
  OUTPUT_FILE="voice_${VOICE}.mp3"

  echo -e "${CYAN}Voice: $VOICE${NC} ($DESC)"

  curl -s -X POST "$API_URL" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $OPENAI_API_KEY" \
    -d @- <<EOF > "$OUTPUT_FILE"
{
  "model": "gpt-4o-mini-tts",
  "input": "$COMPARISON_TEXT",
  "voice": "$VOICE"
}
EOF

  if [ -f "$OUTPUT_FILE" ]; then
    FILE_SIZE=$(wc -c < "$OUTPUT_FILE")
    echo -e "  ✓ Created: $OUTPUT_FILE ($FILE_SIZE bytes)"
  fi
  echo ""
done

################################################################################
# Demo 3: Instructions Parameter
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 3: Using Instructions for Voice Customization${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

SCRIPT_TEXT="The quarterly results exceeded expectations."

# Test different instruction styles
echo -e "${YELLOW}Same text, different instructions:${NC}"
echo ""

# Example 1: Enthusiastic delivery
echo -e "${CYAN}1. Enthusiastic delivery${NC}"
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<'EOF' > "instruction_enthusiastic.mp3"
{
  "model": "gpt-4o-mini-tts",
  "input": "The quarterly results exceeded expectations.",
  "voice": "nova",
  "instructions": "Speak with enthusiasm and excitement, emphasizing positive words."
}
EOF
echo "  ✓ Created: instruction_enthusiastic.mp3"
echo ""

# Example 2: Serious, professional
echo -e "${CYAN}2. Serious, professional delivery${NC}"
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<'EOF' > "instruction_professional.mp3"
{
  "model": "gpt-4o-mini-tts",
  "input": "The quarterly results exceeded expectations.",
  "voice": "onyx",
  "instructions": "Speak in a serious, professional tone suitable for a boardroom presentation."
}
EOF
echo "  ✓ Created: instruction_professional.mp3"
echo ""

# Example 3: Storytelling
echo -e "${CYAN}3. Storytelling style${NC}"
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<'EOF' > "instruction_storytelling.mp3"
{
  "model": "gpt-4o-mini-tts",
  "input": "Once upon a time, in a land far away, there lived a curious explorer.",
  "voice": "fable",
  "instructions": "Speak as if narrating a fairy tale, with dramatic pauses and expressive intonation."
}
EOF
echo "  ✓ Created: instruction_storytelling.mp3"
echo ""

################################################################################
# Demo 4: Speed Control
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 4: Speed Variations${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

SPEED_TEXT="This sentence demonstrates different playback speeds."

# Test different speeds
declare -a SPEEDS=("0.5" "1.0" "1.5" "2.0")

echo -e "${YELLOW}Generating samples at different speeds:${NC}"
echo ""

for SPEED in "${SPEEDS[@]}"; do
  OUTPUT_FILE="speed_${SPEED}x.mp3"

  echo -e "${CYAN}Speed: ${SPEED}x${NC}"

  curl -s -X POST "$API_URL" \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $OPENAI_API_KEY" \
    -d @- <<EOF > "$OUTPUT_FILE"
{
  "model": "gpt-4o-mini-tts",
  "input": "$SPEED_TEXT",
  "voice": "alloy",
  "speed": $SPEED
}
EOF

  if [ -f "$OUTPUT_FILE" ]; then
    echo "  ✓ Created: $OUTPUT_FILE"
  fi
  echo ""
done

################################################################################
# Demo 5: Multi-Language Support
################################################################################

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Demo 5: Multi-Language Support${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

echo -e "${YELLOW}TTS automatically detects and speaks multiple languages:${NC}"
echo ""

# Spanish
echo -e "${CYAN}Spanish:${NC}"
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<'EOF' > "lang_spanish.mp3"
{
  "model": "gpt-4o-mini-tts",
  "input": "Hola, bienvenido a la demostración de síntesis de voz.",
  "voice": "coral"
}
EOF
echo "  ✓ Created: lang_spanish.mp3"
echo ""

# French
echo -e "${CYAN}French:${NC}"
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<'EOF' > "lang_french.mp3"
{
  "model": "gpt-4o-mini-tts",
  "input": "Bonjour, bienvenue à la démonstration de synthèse vocale.",
  "voice": "ballad"
}
EOF
echo "  ✓ Created: lang_french.mp3"
echo ""

# German
echo -e "${CYAN}German:${NC}"
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<'EOF' > "lang_german.mp3"
{
  "model": "gpt-4o-mini-tts",
  "input": "Hallo, willkommen zur Sprachsynthese-Demonstration.",
  "voice": "echo"
}
EOF
echo "  ✓ Created: lang_german.mp3"
echo ""

################################################################################
# Cost Calculation
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Cost Analysis${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

TEXT_LENGTH=${#TEXT}
ESTIMATED_INPUT_TOKENS=$TEXT_LENGTH

# Pricing
INPUT_COST_PER_1M=0.60
OUTPUT_COST_PER_1M=12.00

# Calculate cost (input only - output varies by audio length)
INPUT_COST=$(echo "scale=8; $ESTIMATED_INPUT_TOKENS * $INPUT_COST_PER_1M / 1000000" | bc)

echo -e "${YELLOW}Pricing (gpt-4o-mini-tts):${NC}"
echo "  Input:  \$0.60 per 1M tokens"
echo "  Output: \$12.00 per 1M tokens (audio duration-based)"
echo ""

echo -e "${YELLOW}This demo:${NC}"
echo "  Text length: $TEXT_LENGTH characters"
echo "  Estimated input tokens: $ESTIMATED_INPUT_TOKENS"
echo "  Input cost: \$$INPUT_COST"
echo "  Note: Output cost varies by audio duration"
echo ""

echo -e "${YELLOW}Cost comparison:${NC}"
echo "  100 chars:   ~\$0.00006 input"
echo "  1,000 chars: ~\$0.0006 input"
echo "  10,000 chars: ~\$0.006 input"
echo ""
echo -e "${GREEN}Much more affordable than alternatives!${NC}"
echo ""

################################################################################
# Summary
################################################################################

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "✓ Generated audio with all 11 voices"
echo "✓ Demonstrated instructions parameter for voice customization"
echo "✓ Showed speed control (0.5x to 2.0x)"
echo "✓ Tested multi-language support"
echo "✓ Affordable pricing: \$0.60/1M input tokens"
echo ""

echo -e "${YELLOW}To play audio files:${NC}"
echo "  mpv voice_alloy.mp3"
echo "  afplay voice_nova.mp3  (macOS)"
echo ""

echo -e "${YELLOW}Voice selection guide:${NC}"
echo "  Professional: echo, onyx, sage"
echo "  Friendly: coral, nova, shimmer"
echo "  Storytelling: fable, ballad, verse"
echo "  Neutral: alloy, ash"
echo ""
