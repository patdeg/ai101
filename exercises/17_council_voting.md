# Exercise 17: Council Voting - Multi-Persona Content Selection

[← Previous: Text to Speech](15_text_to_speech.md) | [Back to Exercises](README.md)

## What You'll Learn

- Using the Demeterics Council API for content evaluation
- Multi-persona voting to select the best option
- Leveraging diverse perspectives for decision-making
- Understanding vote tallying and consensus metrics

## What is Council Voting?

The Council API gathers diverse perspectives from AI personas representing different segments of the population. When you ask a question that requires choosing between options, each persona:

1. **Evaluates** each option from their unique perspective
2. **Votes** for their preferred choice
3. **Explains** their reasoning based on their emotions, experience, and worldview

The API then tallies votes and determines a winner, helping you make data-driven content decisions.

## Getting Started

You need a Demeterics API key and a Groq API key (BYOK):
```bash
export DEMETERICS_API_KEY='dmt_your_key'
export GROQ_API_KEY='gsk_your_groq_key'
```

## Exercise 17.1: Basic Story Selection

Ask the council to pick the best story for a short video:

**The Stories (December 3rd in History):**

```
A) John Paul Jones raises the first American flag on a warship in 1775.
   Cannons fire, sailors cheer, the Grand Union Flag flies for the first time.

B) Dr. Christiaan Barnard performs the first human heart transplant in 1967.
   A tense operating room, surgeons working, the transplanted heart begins to beat.

C) Astronomer Charles Perrine discovers Jupiter's moon Himalia in 1904.
   Peering through a telescope, a faint speck moves, expanding humanity's map of space.

D) Edith Sampson becomes the first African-American female judge in 1962.
   The gavel slams, the courtroom applauds, history is made in Chicago.
```

**Python:**
```python
import requests

response = requests.post(
    "https://api.demeterics.com/council/v1/evaluate",
    headers={"Authorization": f"Bearer {dmt_key};{groq_key}"},
    json={
        "question": "Which story would you most want to watch as a short video?",
        "content": stories,  # The 4 stories above
        "num_personas": 8
    }
)

result = response.json()
print(f"Winner: {result['stats']['majority_vote']}")
print(f"Consensus: {result['stats']['vote_consensus']}")
```

**Experiments to try:**

1. **Observe voting patterns:**
   - Which demographics prefer which stories?
   - Do younger personas vote differently than older ones?

2. **Check the vote breakdown:**
   - Was it a close race or a landslide?
   - What does `vote_consensus` tell you?

3. **Read the vote reasons:**
   - Each persona explains WHY they voted
   - Different perspectives reveal different appeals

## Exercise 17.2: Understanding Persona Perspectives

Each persona votes based on their unique worldview:

| Persona | Background | Typical Focus |
|---------|-----------|---------------|
| Maya | 19yo Gen Z, NYC | Visual appeal, authenticity |
| Carlos | 23yo mechanic, Texas | Practical stories, real heroes |
| Kevin | 27yo AI engineer | Logic, data, precision |
| Susan | 60yo retired teacher | Historical significance, education |
| Editor | Professional editor | Structure, clarity, engagement |

**Key insight:** The same content can win or lose depending on your audience!

**Experiments:**

1. **Run the same stories with different persona counts:**
   - 4 personas vs 12 personas
   - Does the winner change?

2. **Look at individual vote reasons:**
   ```python
   for persona in result['persona_responses']:
       print(f"{persona['name']}: {persona['vote']} - {persona['vote_reason']}")
   ```

3. **Identify patterns:**
   - Technical personas may prefer the Jupiter discovery
   - History lovers may prefer the flag story
   - Emotional personas may prefer the heart transplant

## Exercise 17.3: Use Cases for Voting

The voting feature works for any content selection:

**Marketing Headlines:**
```json
{
  "question": "Which headline would make you click to read more?",
  "content": "A) Scientists Discover New Planet\nB) Local Hero Saves Family\nC) Tech Giant Reveals AI Breakthrough",
  "num_personas": 10
}
```

**Product Names:**
```json
{
  "question": "Which product name sounds most appealing?",
  "content": "A) SwiftSync\nB) CloudBridge\nC) DataPulse\nD) FlowForge",
  "num_personas": 12
}
```

**Video Thumbnails (described):**
```json
{
  "question": "Which thumbnail would make you click on a YouTube video?",
  "content": "A) Red shocked face emoji, bold yellow text 'YOU WON'T BELIEVE THIS'\nB) Clean minimalist design, product photo, subtle logo\nC) Before/after split image, dramatic arrow pointing right",
  "num_personas": 8
}
```

## Exercise 17.4: Interpreting Results

The response includes rich voting data:

```json
{
  "stats": {
    "vote_breakdown": {"A": 5, "B": 2, "C": 1},
    "majority_vote": "A",
    "vote_consensus": "63%"
  },
  "persona_responses": [
    {
      "name": "Maya",
      "vote": "A",
      "vote_reason": "The flag moment is super visual and iconic.",
      "interested_level": 82
    }
  ]
}
```

**Key metrics:**

| Metric | Meaning |
|--------|---------|
| `vote_breakdown` | Raw vote counts per option |
| `majority_vote` | The winning option |
| `vote_consensus` | % of votes for winner (higher = stronger) |
| `interested_level` | Overall quality rating (0-100) |

**Interpreting consensus:**
- **80%+**: Strong consensus, clear winner
- **50-80%**: Moderate consensus, consider runner-up
- **<50%**: Split decision, content may need rework

## Exercise 17.5: Advanced - Iterative Refinement

Use voting to iteratively improve content:

1. **Round 1:** Test 4 initial options
2. **Analyze:** Which won? What did voters like?
3. **Round 2:** Create 4 variations of the winner
4. **Repeat:** Until consensus is strong

**Example workflow:**
```python
# Round 1: Broad concepts
stories_v1 = ["Flag story", "Heart transplant", "Jupiter discovery", "Judge story"]
winner_v1 = run_council_vote(stories_v1)

# Round 2: Variations of winner
if winner_v1 == "A":
    stories_v2 = [
        "Flag story - focus on John Paul Jones",
        "Flag story - focus on the sailors",
        "Flag story - focus on the cannon fire",
        "Flag story - focus on the flag itself"
    ]
    winner_v2 = run_council_vote(stories_v2)
```

## Reflection Questions

1. How do different personas interpret the same content?
2. When should you trust a 51% consensus vs. require 80%?
3. How can voting help you understand your audience?
4. What content decisions could you automate with council voting?

## Cost Considerations

- Each council evaluation costs ~$0.003-0.005
- More personas = more accurate but higher cost
- Use 8 personas for most decisions
- Use 12-18 for important content choices

## Security Note

The Council API tracks all evaluations for observability. Never include sensitive personal data in content being evaluated.

---

**Related Examples:**
- [python/17_council_voting.py](../python/17_council_voting.py)
- [nodejs/17_council_voting.js](../nodejs/17_council_voting.js)
- [go/17_council_voting.go](../go/17_council_voting.go)
- [bash/17_council_voting.sh](../bash/17_council_voting.sh)

**See also:**
- [Council API Documentation](https://demeterics.ai/docs/council)
- [Exercise 1: Basic Chat](01_basic_chat.md) - API fundamentals

[← Previous: Text to Speech](15_text_to_speech.md) | [Back to Exercises](README.md)
