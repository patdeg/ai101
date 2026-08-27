package main

/*
Example 15: Text-to-Speech with Fish Audio (via OpenRouter)

Demonstrates:
- Converting text to spoken audio using OpenRouter's /audio/speech endpoint
- Fish Audio S2.1 Pro Free - a genuinely free TTS model, no card required
- Picking a voice by reference ID from Fish Audio's voice library

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

type fishAudioOptions struct {
	ReferenceID string `json:"referenceId"`
}

type providerOptions struct {
	FishAudio fishAudioOptions `json:"fishAudio"`
}

type ttsRequest struct {
	Model           string           `json:"model"`
	Input           string           `json:"input"`
	ResponseFormat  string           `json:"response_format"`
	ProviderOptions *providerOptions `json:"providerOptions,omitempty"`
}

func textToSpeech(text, referenceID string) ([]byte, error) {
	request := ttsRequest{
		Model:          "fish-audio/s2.1-pro-free:free",
		Input:          text,
		ResponseFormat: "mp3",
	}
	// Without a referenceId, Fish Audio picks a default voice - good enough
	// for a first TTS demo. Pass one to pick a specific voice (see Example 16
	// for the multi-speaker version of this).
	if referenceID != "" {
		request.ProviderOptions = &providerOptions{FishAudio: fishAudioOptions{ReferenceID: referenceID}}
	}

	requestBody, err := json.Marshal(request)
	if err != nil {
		return nil, err
	}

	req, err := http.NewRequest("POST", "https://openrouter.ai/api/v1/audio/speech", bytes.NewBuffer(requestBody))
	if err != nil {
		return nil, err
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %s", os.Getenv("OPENROUTER_API_KEY")))

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}
	if resp.StatusCode != 200 {
		return nil, fmt.Errorf("API Error: HTTP %d\n%s", resp.StatusCode, body)
	}
	return body, nil
}

func main() {
	if os.Getenv("OPENROUTER_API_KEY") == "" {
		fmt.Fprintln(os.Stderr, "Error: OPENROUTER_API_KEY not set")
		fmt.Fprintln(os.Stderr, "Get your key from: https://openrouter.ai/keys")
		os.Exit(1)
	}

	fmt.Println("========================================")
	fmt.Println("Text-to-Speech with Fish Audio (OpenRouter)")
	fmt.Println("========================================")
	fmt.Println()

	// Demo 1: Basic TTS with the default voice
	text := "Hello! This is an example of text-to-speech synthesis using Fish Audio's free TTS model."
	audio, err := textToSpeech(text, "")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

	os.WriteFile("demo1_default_voice.mp3", audio, 0644)
	fmt.Printf("✓ Audio created: demo1_default_voice.mp3 (%d bytes)\n\n", len(audio))

	// Demo 2: A specific voice, if the learner has picked one
	if referenceID := os.Getenv("FISH_VOICE_1"); referenceID != "" {
		comparisonText := "This is the same text, spoken with a voice picked from Fish Audio's library."
		audio, err := textToSpeech(comparisonText, referenceID)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
		os.WriteFile("demo2_chosen_voice.mp3", audio, 0644)
		fmt.Printf("✓ Chosen voice: demo2_chosen_voice.mp3 (%d bytes)\n\n", len(audio))
	} else {
		fmt.Println("Tip: set FISH_VOICE_1 to a voice ID from https://fish.audio to")
		fmt.Println("hear a specific voice instead of the default.")
		fmt.Println()
	}

	fmt.Println("========================================")
	fmt.Println("Cost Analysis")
	fmt.Println("========================================")
	fmt.Println()
	fmt.Println("Pricing: fish-audio/s2.1-pro-free:free costs $0 (rate-limited,")
	fmt.Println("best-effort availability - meant for learning/prototyping)")
	fmt.Println()
}
