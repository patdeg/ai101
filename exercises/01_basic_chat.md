# Exercise 1: Basic Chat - Understanding AI Responses

[← Back to Exercises](README.md) | [Next: System Prompt →](02_system_prompt.md)

## What You Learned

- Basic API calls
- JSON structure
- Authentication
- Response parsing

## Exercise 1.1: Temperature Exploration

Run the same question 5 times with different temperature values:
- `"temperature": 0.0` (most deterministic/focused)
- `"temperature": 0.5` (balanced)
- `"temperature": 1.0` (default, creative)
- `"temperature": 1.5` (very creative)
- `"temperature": 2.0` (maximum randomness)

**Question to ask:** "Explain what a computer does in one sentence."

**What to observe:** How does the answer change? Which temperature gives the most consistent results? Which gives the most variety?

**Extension:** Try the same with a creative task: "Write a haiku about programming." Notice how temperature affects creativity vs. factual responses differently.

## Exercise 1.2: Token Limits and Truncation

Add `"max_completion_tokens": N` to your request and experiment:
- Set to 10 tokens - What happens to the response?
- Set to 50 tokens - Is the answer complete?
- Set to 500 tokens - Does the AI use all tokens?

**Question to ask:** "Explain how the internet works."

**What to learn:** Understand how token limits work, when answers get cut off (finish_reason: "length"), and how to estimate tokens needed.

## Exercise 1.3: Top-P Nucleus Sampling

Compare `top_p` values with fixed temperature:
- `"temperature": 1.0, "top_p": 0.1` (very focused word choices)
- `"temperature": 1.0, "top_p": 0.5` (moderate variety)
- `"temperature": 1.0, "top_p": 0.9` (diverse word choices)
- `"temperature": 1.0, "top_p": 1.0` (default, all words considered)

**What to learn:** How `top_p` filters word choices differently than temperature. Try to understand the difference between randomness (temperature) and vocabulary restriction (top_p).

## Exercise 1.4: Cost Tracking and Estimation

Create a simple log of your API calls:
- Track prompt_tokens, completion_tokens for 10 different questions
- Calculate cost using current pricing ($0.11/1M input, $0.34/1M output)
- Try to predict token count before running (hint: ~1 token ≈ 4 characters)

**What to learn:** Real-world API cost management and token estimation skills.

## Exercise 1.5: Stop Sequences

Add `"stop": ["\n", ".", "!"]` to make the AI stop at specific characters:
- Stop at first period: `"stop": ["."]`
- Stop at newline: `"stop": ["\n"]`
- Stop at custom word: `"stop": ["therefore", "because"]`

**Question to try:** "List 5 programming languages:" with `"stop": ["\n"]` - what happens?

**What to learn:** Control over response length and format using stop sequences.

## Exercise 1.6: Multi-Vendor Comparison

Run the SAME question across different AI providers and compare:

**Task:** Ask each provider: "What makes a good programmer?"

Run with different providers:
```bash
# Bash examples
./01_basic_chat_minimal.sh           # Groq
./01_basic_chat_OPENAI_minimal.sh    # OpenAI
./01_basic_chat_ANTHROPIC_minimal.sh # Anthropic
./01_basic_chat_SAMBA_minimal.sh     # SambaNova
./01_basic_chat_DEMETERICS_minimal.sh # Via proxy
```

**What to compare:**
- Response style and tone
- Answer length and detail
- Response time (latency)
- Token usage and cost
- Unique perspectives each model provides

**Track in a table:**
| Provider | Response Time | Tokens Used | Cost | Key Points |
|----------|--------------|-------------|------|------------|
| Groq     | ...          | ...         | ...  | ...        |
| OpenAI   | ...          | ...         | ...  | ...        |
| Anthropic| ...          | ...         | ...  | ...        |

**What to learn:** Different AI providers have different strengths. Groq is fast, OpenAI is advanced, Anthropic is nuanced, SambaNova uses open models, and Demeterics adds observability.

## Exercise 1.7: Observability with Demeterics

Use the Demeterics proxy to track your API usage:

1. Run 10 different questions through Demeterics
2. Visit the Demeterics dashboard at https://demeterics.com/dashboard
3. Analyze:
   - Total tokens used
   - Average response time
   - Cost breakdown
   - Error rates (if any)

**What to learn:** Production monitoring and analytics are crucial for AI applications. Demeterics provides Google Analytics-style insights for your AI API calls.

## Reflection Questions

After completing these exercises, consider:

1. When would you use low temperature vs. high temperature?
2. How can you estimate API costs before making calls?
3. What's the trade-off between `temperature` and `top_p`?
4. When might stop sequences be useful in real applications?

## Next Steps

Once you've mastered basic parameters, move on to [Exercise 2: System Prompt](02_system_prompt.md) to learn how to control AI behavior.

---

**Related Examples:**
- **Bash:** [Groq](../bash/01_basic_chat_minimal.sh) | [OpenAI](../bash/01_basic_chat_OPENAI_minimal.sh) | [Anthropic](../bash/01_basic_chat_ANTHROPIC_minimal.sh) | [SambaNova](../bash/01_basic_chat_SAMBA_minimal.sh) | [Demeterics](../bash/01_basic_chat_DEMETERICS_minimal.sh)
- **Node.js:** [Groq](../nodejs/01_basic_chat.js) | [OpenAI](../nodejs/01_basic_chat_OPENAI.js) | [Anthropic](../nodejs/01_basic_chat_ANTHROPIC.js) | [SambaNova](../nodejs/01_basic_chat_SAMBA.js) | [Demeterics](../nodejs/01_basic_chat_DEMETERICS.js)
- **Python:** [Groq](../python/01_basic_chat.py) | [OpenAI](../python/01_basic_chat_OPENAI.py) | [Anthropic](../python/01_basic_chat_ANTHROPIC.py) | [SambaNova](../python/01_basic_chat_SAMBA.py) | [Demeterics](../python/01_basic_chat_DEMETERICS.py)
- **Go:** [Groq](../go/01_basic_chat.go) | [OpenAI](../go/01_basic_chat_OPENAI.go)

[← Back to Exercises](README.md) | [Next: System Prompt →](02_system_prompt.md)
