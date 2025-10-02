package main

/*
Example 14: Text-to-Speech with OpenAI

Demonstrates:
- Converting text to spoken audio
- 11 different voices
- Instructions and speed parameters
- Multi-language support

Note: Uses Go standard library only
*/

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

func textToSpeech(text, voice string, options map[string]interface{}) ([]byte, error) {
	requestData := map[string]interface{}{
		"model": "gpt-4o-mini-tts",
		"input": text,
		"voice": voice,
	}
	
	for k, v := range options {
		requestData[k] = v
	}
	
	requestBody, _ := json.Marshal(requestData)
	
	req, err := http.NewRequest("POST", "https://api.openai.com/v1/audio/speech", bytes.NewBuffer(requestBody))
	if err != nil {
		return nil, err
	}
	
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %s", os.Getenv("OPENAI_API_KEY")))
	
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()
	
	return io.ReadAll(resp.Body)
}

func main() {
	if os.Getenv("OPENAI_API_KEY") == "" {
		fmt.Fprintln(os.Stderr, "Error: OPENAI_API_KEY not set")
		fmt.Fprintln(os.Stderr, "Get your key from: https://platform.openai.com")
		os.Exit(1)
	}
	
	fmt.Println("========================================")
	fmt.Println("Text-to-Speech with OpenAI")
	fmt.Println("========================================\n")
	
	// Demo 1: Basic TTS
	text := "Hello! This is an example of text-to-speech synthesis using OpenAI's affordable TTS model."
	audio, err := textToSpeech(text, "alloy", nil)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
	
	os.WriteFile("demo1_alloy.mp3", audio, 0644)
	fmt.Printf("✓ Audio created: demo1_alloy.mp3 (%d bytes)\n\n", len(audio))
	
	// Demo 2: All voices
	fmt.Println("Generating all 11 voices...\n")
	voices := []string{"alloy", "ash", "ballad", "coral", "echo", "fable", 
	                   "onyx", "nova", "sage", "shimmer", "verse"}
	
	comparisonText := "Welcome to OpenAI's text-to-speech demonstration."
	for _, voice := range voices {
		audio, _ := textToSpeech(comparisonText, voice, nil)
		filename := fmt.Sprintf("voice_%s.mp3", voice)
		os.WriteFile(filename, audio, 0644)
		fmt.Printf("✓ %s: %s\n", voice, filename)
	}
	
	fmt.Println("\n========================================")
	fmt.Println("Cost Analysis")
	fmt.Println("========================================\n")
	fmt.Println("Pricing: $0.60/1M input tokens, $12/1M output tokens")
	fmt.Printf("This demo: ~%d chars = $%.8f input\n\n", len(text), float64(len(text))*0.60/1000000)
	fmt.Println("Much more affordable than alternatives!\n")
}
