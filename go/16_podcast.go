////////////////////////////////////////////////////////////////////////////////
// Example 16: Multi-Speaker Podcast Generation
////////////////////////////////////////////////////////////////////////////////
//
// This example demonstrates how to generate a podcast with multiple speakers
// using Demeterics multi-speaker TTS API in a SINGLE API call.
//
// What you'll learn:
// - Multi-speaker audio generation with distinct voices
// - NPR/NotebookLM "Deep Dive" podcast style
// - Voice selection from 30 available options
// - Script formatting with "Speaker: text" pattern
//
// Prerequisites:
// - DEMETERICS_API_KEY environment variable
//
// Usage:
//   export DEMETERICS_API_KEY="dmt_your-api-key"
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
)

// Step 1: Define request structures (Demeterics format)
type Speaker struct {
	Name  string `json:"name"`
	Voice string `json:"voice"`
}

type Request struct {
	Provider string    `json:"provider"`
	Input    string    `json:"input"`
	Speakers []Speaker `json:"speakers"`
}

func main() {
	// Step 2: Check for API key
	apiKey := os.Getenv("DEMETERICS_API_KEY")
	if apiKey == "" {
		fmt.Println("Error: DEMETERICS_API_KEY not set")
		fmt.Println("Get your key from: https://demeterics.com")
		os.Exit(1)
	}

	// Step 3: Define the podcast script
	// Format: "SpeakerName: dialogue text"
	// Speaker names must match exactly in speakers array
	podcastScript := `Alex: Hey, welcome back to The Deep Dive! I am Alex.
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
Sam: See you next time!`

	// Step 4: Configure voices for each speaker
	// Available voices (30 total):
	//   Puck (upbeat), Kore (firm), Charon (informative), Zephyr (bright),
	//   Fenrir (excitable), Leda (youthful), Aoede (breezy), Sulafat (warm),
	//   Achird (friendly), and 21 others.
	speakers := []Speaker{
		{Name: "Alex", Voice: "Charon"}, // Informative, clear - explains concepts
		{Name: "Sam", Voice: "Puck"},    // Upbeat - energetic, enthusiastic
	}

	fmt.Println("========================================")
	fmt.Println("Multi-Speaker Podcast Generation")
	fmt.Println("========================================")
	fmt.Println("")
	fmt.Println("Speakers:")
	for _, s := range speakers {
		fmt.Printf("  %s: %s\n", s.Name, s.Voice)
	}
	fmt.Println("")
	fmt.Printf("Script length: %d characters\n", len(podcastScript))
	fmt.Println("")
	fmt.Println("Generating podcast audio...")

	// Step 5: Build the request (Demeterics format)
	request := Request{
		Provider: "gemini",
		Input:    podcastScript,
		Speakers: speakers,
	}

	requestBody, err := json.Marshal(request)
	if err != nil {
		fmt.Printf("Error marshaling request: %v\n", err)
		os.Exit(1)
	}

	// Step 6: Make the API request
	url := "https://api.demeterics.com/tts/v1/generate"
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

	// Step 8: Save the audio file directly (Demeterics returns audio bytes)
	audioBytes, err := io.ReadAll(resp.Body)
	if err != nil {
		fmt.Printf("Error reading response: %v\n", err)
		os.Exit(1)
	}

	outputFile := "columbus_podcast.wav"
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
}
