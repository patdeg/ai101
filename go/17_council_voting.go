// Exercise 17: Council Voting - Multi-Persona Content Selection
//
// Demonstrates:
// - Demeterics Council API for content evaluation
// - Multi-persona voting to select the best option
// - Vote tallying and consensus metrics
package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"sort"
	"strings"
)

// Stories from December 3rd in History (simplified)
const stories = `A) John Paul Jones raises the first American flag on a warship in 1775.
Cannons fire over Boston Harbor as sailors cheer. The Grand Union Flag flies
for the first time, marking the birth of the American naval tradition.

B) Dr. Christiaan Barnard performs the first human heart transplant in 1967.
In a tense operating room at Groote Schuur Hospital, surgeons lift a donor
heart. When it begins to beat in the patient's chest, medical history is made.

C) Astronomer Charles Perrine discovers Jupiter's moon Himalia in 1904.
Peering through the telescope at Lick Observatory, a faint speck moves against
Jupiter's clouds. Humanity's map of the solar system expands once more.

D) Edith Sampson becomes the first African-American female judge in 1962.
The gavel slams in a packed Chicago courtroom. Cameras flash as Sampson takes
the bench, a watershed moment for American justice and civil rights.`

// CouncilRequest represents the API request
type CouncilRequest struct {
	Question    string `json:"question"`
	Content     string `json:"content"`
	NumPersonas int    `json:"num_personas"`
}

// PersonaResponse represents a single persona's evaluation
type PersonaResponse struct {
	Name            string   `json:"name"`
	Description     string   `json:"description"`
	InterestedLevel int      `json:"interested_level"`
	Vote            string   `json:"vote"`
	VoteReason      string   `json:"vote_reason"`
	Guidance        string   `json:"guidance"`
	ClarityFlags    []string `json:"clarity_flags"`
}

// CouncilStats contains voting statistics
type CouncilStats struct {
	TotalPersonas          int            `json:"total_personas"`
	SuccessfulResponses    int            `json:"successful_responses"`
	AverageInterestedLevel float64        `json:"average_interested_level"`
	VoteBreakdown          map[string]int `json:"vote_breakdown"`
	MajorityVote           string         `json:"majority_vote"`
	VoteConsensus          string         `json:"vote_consensus"`
}

// UsageInfo contains billing information
type UsageInfo struct {
	TotalCallCount int     `json:"total_call_count"`
	TotalTokens    int     `json:"total_tokens"`
	TotalCostUSD   float64 `json:"total_cost_usd"`
}

// CouncilResponse represents the API response
type CouncilResponse struct {
	ID               string            `json:"id"`
	Score            int               `json:"score"`
	Summary          string            `json:"summary"`
	PersonaResponses []PersonaResponse `json:"persona_responses"`
	Stats            CouncilStats      `json:"stats"`
	Usage            UsageInfo         `json:"usage"`
	Error            *struct {
		Message string `json:"message"`
	} `json:"error"`
}

func callCouncil(question, content string, numPersonas int) (*CouncilResponse, error) {
	demetericsKey := os.Getenv("DEMETERICS_API_KEY")
	groqKey := os.Getenv("GROQ_API_KEY")

	if demetericsKey == "" || groqKey == "" {
		return nil, fmt.Errorf("both DEMETERICS_API_KEY and GROQ_API_KEY must be set")
	}

	// Use dual-key format: demeterics_key;groq_key
	authHeader := fmt.Sprintf("%s;%s", demetericsKey, groqKey)

	reqBody := CouncilRequest{
		Question:    question,
		Content:     content,
		NumPersonas: numPersonas,
	}

	jsonBody, err := json.Marshal(reqBody)
	if err != nil {
		return nil, fmt.Errorf("failed to marshal request: %w", err)
	}

	req, err := http.NewRequest("POST", "https://api.demeterics.com/council/v1/evaluate", bytes.NewBuffer(jsonBody))
	if err != nil {
		return nil, fmt.Errorf("failed to create request: %w", err)
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+authHeader)

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return nil, fmt.Errorf("failed to send request: %w", err)
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("failed to read response: %w", err)
	}

	var result CouncilResponse
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, fmt.Errorf("failed to parse response: %w", err)
	}

	return &result, nil
}

func displayResults(result *CouncilResponse) {
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println("COUNCIL VOTING RESULTS")
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println()

	// Check for errors
	if result.Error != nil {
		fmt.Printf("Error: %s\n", result.Error.Message)
		return
	}

	// Winner and consensus
	fmt.Printf("Winner: %s\n", result.Stats.MajorityVote)
	fmt.Printf("Consensus: %s\n", result.Stats.VoteConsensus)
	fmt.Println()

	// Vote breakdown (sorted by vote count)
	fmt.Println("Vote Breakdown:")
	type voteEntry struct {
		option string
		count  int
	}
	var votes []voteEntry
	for option, count := range result.Stats.VoteBreakdown {
		votes = append(votes, voteEntry{option, count})
	}
	sort.Slice(votes, func(i, j int) bool {
		return votes[i].count > votes[j].count
	})
	for _, v := range votes {
		bar := strings.Repeat("*", v.count)
		fmt.Printf("  %s: %s (%d votes)\n", v.option, bar, v.count)
	}
	fmt.Println()

	// Overall score
	fmt.Printf("Average Interest Level: %.1f/100\n", result.Stats.AverageInterestedLevel)
	fmt.Printf("Personas: %d/%d responded\n", result.Stats.SuccessfulResponses, result.Stats.TotalPersonas)
	fmt.Println()

	// Individual votes
	fmt.Println("Individual Votes:")
	fmt.Println(strings.Repeat("-", 60))
	for _, persona := range result.PersonaResponses {
		fmt.Printf("  %s: %s\n", persona.Name, persona.Vote)
		fmt.Printf("    Reason: %s\n", persona.VoteReason)
		fmt.Printf("    Interest: %d/100\n", persona.InterestedLevel)
		fmt.Println()
	}

	// Summary
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println("COUNCIL SUMMARY")
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println(result.Summary)
	fmt.Println()

	// Usage info
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println("USAGE")
	fmt.Println(strings.Repeat("=", 60))
	fmt.Printf("Total API calls: %d\n", result.Usage.TotalCallCount)
	fmt.Printf("Total tokens: %d\n", result.Usage.TotalTokens)
	fmt.Printf("Cost: $%.4f\n", result.Usage.TotalCostUSD)
}

func main() {
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println("Council Voting Demo - Best Story Selection")
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println()
	fmt.Println("Question: Which story would you most want to watch as a short video?")
	fmt.Println()
	fmt.Println("Stories:")
	fmt.Println(stories)
	fmt.Println()
	fmt.Println("Sending to Council API with 8 personas...")
	fmt.Println()

	result, err := callCouncil(
		"Which story would you most want to watch as a short video?",
		stories,
		8,
	)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

	displayResults(result)
}
