////////////////////////////////////////////////////////////////////////////////
// Example 16: Multi-Speaker Podcast Generation
////////////////////////////////////////////////////////////////////////////////
//
// This example demonstrates how to generate a podcast with multiple speakers
// using OpenRouter's audio/speech endpoint (Fish Audio S2.1 Pro Free model)
// in a SINGLE API call.
//
// What you'll learn:
// - Multi-speaker audio generation with distinct voices
// - NPR/NotebookLM "Deep Dive" podcast style
// - Speaker turns marked with <|speaker:N|> tags in the input text
// - Script formatting with "Speaker: text" pattern
//
// Prerequisites:
// - OPENROUTER_API_KEY environment variable (free at https://openrouter.ai/keys)
// - Two Fish Audio voice reference IDs (free account at https://fish.audio ->
//   browse the voice library -> copy a voice's ID), set as FISH_VOICE_1 and
//   FISH_VOICE_2. The model still works with a single default voice if you
//   only want to experiment with one speaker (see example 15).
//
// Usage:
//   export OPENROUTER_API_KEY="your-api-key"
//   export FISH_VOICE_1="<voice-id-for-alex>"
//   export FISH_VOICE_2="<voice-id-for-sam>"
//   go run 16_podcast.go
//
////////////////////////////////////////////////////////////////////////////////

package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
)

// Step 1: Define request structures (OpenRouter audio/speech format)
type FishAudioOptions struct {
	ReferenceID []string `json:"referenceId"`
}

type ProviderOptions struct {
	FishAudio FishAudioOptions `json:"fishAudio"`
}

type Request struct {
	Model           string          `json:"model"`
	Input           string          `json:"input"`
	ProviderOptions ProviderOptions `json:"providerOptions"`
	ResponseFormat  string          `json:"response_format"`
}

func main() {
	// Step 2: Check for API key and voice IDs
	apiKey := os.Getenv("OPENROUTER_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: OPENROUTER_API_KEY not set")
		fmt.Println("Get a free key from: https://openrouter.ai/keys")
		os.Exit(1)
	}

	voice1 := os.Getenv("FISH_VOICE_1")
	voice2 := os.Getenv("FISH_VOICE_2")
	if voice1 == "" || voice2 == "" {
		fmt.Println("Error: FISH_VOICE_1 and FISH_VOICE_2 not set")
		fmt.Println("Get free voice IDs from https://fish.audio (browse the voice")
		fmt.Println("library, copy a voice's ID) and export them, e.g.:")
		fmt.Println("  export FISH_VOICE_1=\"...\"")
		fmt.Println("  export FISH_VOICE_2=\"...\"")
		os.Exit(1)
	}

	// Step 3: Define the podcast script
	// Format: "SpeakerName: dialogue text" - Alex speaks first (speaker 0),
	// Sam speaks second (speaker 1). We convert this to <|speaker:N|> tags below.
	type line struct {
		speaker string
		text    string
	}

	script := []line{
		{"Alex", "Hey, welcome back to The Deep Dive! I am Alex."},
		{"Sam", "And I am Sam. Today we are diving into something that every kid learns in school, but honestly, gets way more complicated when you dig into it."},
		{"Alex", "Columbus. Christopher Columbus. 1492. In fourteen hundred ninety two, Columbus sailed the ocean blue."},
		{"Sam", "Right! But here is the thing. That rhyme does not tell you much, does it?"},
		{"Alex", "Not at all! So get this. Columbus was not even trying to discover America. He thought he was finding a shortcut to Asia."},
		{"Sam", "To India, specifically. Which is why he called the native people Indians."},
		{"Alex", "Exactly! A massive geography fail that stuck around for five hundred years."},
		{"Sam", "So let us set the scene. It is 1492. Columbus is Italian, from Genoa, but he is sailing for Spain."},
		{"Alex", "Because Portugal said no! He pitched this idea everywhere. England said no. France said no. Portugal said hard no."},
		{"Sam", "But Queen Isabella and King Ferdinand of Spain said, you know what, sure, let us do it."},
		{"Alex", "And here is what is wild. Columbus was wrong about basically everything. He thought the Earth was way smaller than it actually is."},
		{"Sam", "Most educated people knew the Earth was round. That is a myth that he proved it. They knew. They just thought his math was bad."},
		{"Alex", "Because it was! If America was not there, he and his crew would have starved in the middle of the ocean."},
		{"Sam", "So he gets lucky. Three ships. The Nina, the Pinta, and the Santa Maria. About two months at sea."},
		{"Alex", "And on October 12th, 1492, they land in the Bahamas. Not mainland America. The Bahamas."},
		{"Sam", "An island he named San Salvador. And the people already living there? The Taino people. They had been there for centuries."},
		{"Alex", "So this whole discovery narrative is complicated, to say the least."},
		{"Sam", "Very complicated. It is really the story of a European arriving somewhere that was not empty. And that changes everything."},
		{"Alex", "That is the deep dive for today. Thanks for listening, everyone!"},
		{"Sam", "See you next time!"},
	}

	// Step 4: Assign each named speaker a 0-based index and build the
	// <|speaker:N|> tagged input string Fish Audio expects.
	speakerIndex := map[string]int{"Alex": 0, "Sam": 1}
	var sb strings.Builder
	for _, l := range script {
		fmt.Fprintf(&sb, "<|speaker:%d|>%s ", speakerIndex[l.speaker], l.text)
	}
	podcastScript := strings.TrimSpace(sb.String())

	fmt.Println("========================================")
	fmt.Println("Multi-Speaker Podcast Generation")
	fmt.Println("========================================")
	fmt.Println("")
	fmt.Println("Speakers:")
	fmt.Printf("  Alex (speaker:0) -> voice %s\n", voice1)
	fmt.Printf("  Sam  (speaker:1) -> voice %s\n", voice2)
	fmt.Println("")
	fmt.Printf("Script length: %d characters\n", len(podcastScript))
	fmt.Println("")
	fmt.Println("Generating podcast audio (this is a free model - may be slower")
	fmt.Println("than a paid tier)...")

	// Step 5: Build the request (OpenRouter / Fish Audio format)
	request := Request{
		Model: "fish-audio/s2.1-pro-free:free",
		Input: podcastScript,
		ProviderOptions: ProviderOptions{
			FishAudio: FishAudioOptions{
				ReferenceID: []string{voice1, voice2},
			},
		},
		ResponseFormat: "mp3",
	}

	requestBody, err := json.Marshal(request)
	if err != nil {
		fmt.Printf("Error marshaling request: %v\n", err)
		os.Exit(1)
	}

	// Step 6: Make the API request
	url := "https://openrouter.ai/api/v1/audio/speech"
	req, err := http.NewRequest("POST", url, bytes.NewReader(requestBody))
	if err != nil {
		fmt.Printf("Error creating request: %v\n", err)
		os.Exit(1)
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+apiKey)

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		fmt.Printf("Error making request: %v\n", err)
		os.Exit(1)
	}
	defer resp.Body.Close()

	// Step 7: Check for errors
	if resp.StatusCode != 200 {
		body, _ := io.ReadAll(resp.Body)
		fmt.Printf("API Error: HTTP %d\n", resp.StatusCode)
		fmt.Println(string(body))
		os.Exit(1)
	}

	// Step 8: Save the audio file directly (OpenRouter returns raw audio bytes)
	audioBytes, err := io.ReadAll(resp.Body)
	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	outputFile := "columbus_podcast.mp3"
	if err := os.WriteFile(outputFile, audioBytes, 0644); err != nil {
		fmt.Printf("Error writing file: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("")
	fmt.Println("Success!")
	fmt.Println("")
	fmt.Println("Output:")
	fmt.Printf("  File: %s\n", outputFile)
	fmt.Printf("  Size: %d bytes (%d KB)\n", len(audioBytes), len(audioBytes)/1024)
	fmt.Println("")
	fmt.Println("To play:")
	fmt.Printf("  mpv %s\n", outputFile)
	fmt.Println("")
	fmt.Println("Note: fish-audio/s2.1-pro-free:free is a no-cost, best-effort")
	fmt.Println("tier (no latency/availability guarantees) - fine for learning,")
	fmt.Println("not for production podcasts.")
}
