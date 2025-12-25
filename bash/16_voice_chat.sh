#!/bin/bash

# Prerequisites: sudo apt install sox libsox-fmt-all jq curl

# 1. Configuration
API_KEY="$DEMETERICS_API_KEY"

# Endpoints
STT_URL="https://api.demeterics.com/stt/v1/transcribe"    # Assumed endpoint
CHAT_URL="https://api.demeterics.com/groq/v1/chat/completions"
TTS_URL="https://api.demeterics.com/tts/v1/generate"      # As requested

# Models & Voices
STT_MODEL="whisper-large-v3-turbo"
CHAT_MODEL="meta-llama/llama-4-scout-17b-16e-instruct"
TTS_VOICE="en-US-Standard"  # Example voice ID

# Check for API Key
if [ -z "$API_KEY" ]; then
    echo "Error: DEMETERICS_API_KEY environment variable not set."
    exit 1
fi

echo "----------------------------------------------------"
echo "🎙  Demeterics Voice Agent (Bash Edition)"
echo "----------------------------------------------------"

while true; do
    echo -e "\n🔴 Recording (Press Ctrl+C to stop)..."
    
    # -------------------------------------------------------
    # 1. RECORD AUDIO
    # -------------------------------------------------------
    # Uses SoX to record 16-bit mono wav @ 16k
    # 'silence' effect stops recording after 2.5s of silence
    rec -q -r 16000 -c 1 -b 16 input.wav silence 1 0.1 3% 1 2.5 3%
    
    echo "🔄 Transcribing..."

    # -------------------------------------------------------
    # 2. SPEECH-TO-TEXT (STT)
    # -------------------------------------------------------
    # Assumes multipart/form-data upload similar to OpenAI/Groq standards
    USER_TEXT=$(curl -s -X POST "$STT_URL" \
      -H "Authorization: Bearer $API_KEY" \
      -F "file=@input.wav" \
      -F "model=$STT_MODEL" \
      -F "response_format=json" | jq -r '.text // .transcription')

    # Handle empty transcription
    if [ "$USER_TEXT" == "null" ] || [ -z "$USER_TEXT" ]; then
        echo "⚠️  No speech detected. Trying again..."
        continue
    fi

    echo "🗣  You: $USER_TEXT"

    # -------------------------------------------------------
    # 3. CHAT COMPLETION (LLM)
    # -------------------------------------------------------
    echo "🤖 Thinking..."
    
    AI_RESPONSE=$(curl -s -X POST "$CHAT_URL" \
      -H "Authorization: Bearer $API_KEY" \
      -H "Content-Type: application/json" \
      -d "{
        \"model\": \"$CHAT_MODEL\",
        \"messages\": [
            {\"role\": \"system\", \"content\": \"You are a helpful and concise voice assistant. Keep answers under 2 sentences.\"},
            {\"role\": \"user\", \"content\": \"$USER_TEXT\"}
        ]
      }" | jq -r '.choices[0].message.content')

    echo "🤖 AI: $AI_RESPONSE"

    # -------------------------------------------------------
    # 4. TEXT-TO-SPEECH (TTS)
    # -------------------------------------------------------
    echo "🔊 Generating Audio..."

    # Assumes a standard JSON payload for the 'generate' endpoint
    curl -s -X POST "$TTS_URL" \
      -H "Authorization: Bearer $API_KEY" \
      -H "Content-Type: application/json" \
      -d "{
        \"text\": \"$AI_RESPONSE\",
        \"voice\": \"$TTS_VOICE\",
        \"model\": \"tts-1\"
      }" --output response.mp3

    # Play the response
    play -q response.mp3
done

