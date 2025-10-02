# Exercise 2: System Prompt - Controlling AI Behavior

[← Previous: Basic Chat](01_basic_chat.md) | [Back to Exercises](README.md) | [Next: Vision →](03_vision.md)

## What You Learned

- System vs. user messages
- Behavior control
- Role-playing
- Output formatting

## Exercise 2.1: Persona Experimentation

Try different system prompts with the same user question:
- `"You are a pirate. Speak like a pirate in all responses."`
- `"You are a 5-year-old child. Use simple words."`
- `"You are a Shakespearean poet. Speak in verse."`
- `"You are a grumpy old programmer who hates new technology."`

**User question:** "How do I make a website?"

**What to observe:** How much does the system prompt change the response style vs. content?

## Exercise 2.2: Expertise Level Tuning

Use system prompts to control technical depth:
- `"Explain like I'm 5 years old"`
- `"ELI5"`, `"ELI10"`, `"ELI12"`, ...
- `"Explain like I'm a high school student"`
- `"Explain like I'm a college CS major"`
- `"Explain like I'm a senior software engineer"`

**User question:** "What is recursion?"

**What to learn:** How to tune AI responses for different audiences.

## Exercise 2.3: Output Format Control

Use system prompts to enforce structure:
- `"Always respond with exactly 3 bullet points"`
- `"Number every sentence in your response"`
- `"Respond in haiku format (5-7-5 syllables)"`
- `"Use markdown formatting with headers and lists"`

**What to learn:** System prompts can control format, not just content.

## Exercise 2.4: JSON Mode with response_format Parameter

This exercise introduces **structured output** using the API's built-in JSON mode.

**Important requirement:** When using `response_format: { type: "json_object" }`, the word "JSON" must appear in your system prompt or user message.

**Basic JSON Mode Example:**
```json
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
  "messages": [
    {
      "role": "system",
      "content": "You are a helpful assistant. Respond in JSON format with keys: answer, confidence (0-100), category."
    },
    {
      "role": "user",
      "content": "What is the capital of France?"
    }
  ],
  "response_format": { "type": "json_object" }
}
```

**Experiments to try:**

1. **Simple structured response:**
   - System: `"Respond in JSON format: {answer: string, confidence: number}"`
   - Add: `"response_format": { "type": "json_object" }`
   - Question: "What is 25 + 17?"
   - **Observe:** Does it always return valid JSON?

2. **Complex nested structures:**
   - System: `"Return JSON with this structure: {analysis: {sentiment: string, score: number, keywords: [string]}, summary: string}"`
   - Question: "Analyze this review: 'Great product, fast shipping!'"
   - **Observe:** How well does it follow nested structures?

3. **Without the word "JSON":**
   - System: `"Respond with answer and confidence fields"` (no "JSON" mentioned)
   - Add: `"response_format": { "type": "json_object" }`
   - **Observe:** What happens? Does it still work?

4. **Compare with vs. without response_format:**
   - Try same prompt with and without the `response_format` parameter
   - System: `"Respond in JSON format: {name: string, age: number}"`
   - **Observe:** Does adding the parameter improve JSON validity?

5. **Array responses:**
   - System: `"Respond in JSON format with an array of objects: [{name: string, category: string}]"`
   - Question: "List 3 programming languages with their categories"
   - **Observe:** Can it handle JSON arrays?

**What to learn:**
- Difference between prompt-based formatting and API-enforced formatting
- When `response_format` guarantees valid JSON syntax (but not schema compliance)
- How to design prompts for structured data extraction
- Error handling when JSON parsing fails

**Advanced:** Try combining `response_format` with validation libraries in your code to ensure the JSON matches your expected schema.

**Documentation:** https://console.groq.com/docs/structured-outputs#json-object-mode

## Exercise 2.5: Constraint Testing

Create restrictive system prompts:
- `"Answer in exactly 10 words"`
- `"Never use the letter 'e' in your response"`
- `"Respond using only yes, no, or maybe"`
- `"Give 2 advantages and 2 disadvantages for every answer"`

**What to observe:** When do constraints break? How does the AI handle impossible constraints?

## Exercise 2.6: Multi-turn Conversation

Add conversation history to see context awareness:

```json
{
  "messages": [
    {"role": "system", "content": "You are a helpful math tutor"},
    {"role": "user", "content": "What is 25 + 17?"},
    {"role": "assistant", "content": "42"},
    {"role": "user", "content": "Now multiply that by 3"}
  ]
}
```

**What to learn:** How the AI uses conversation history. Try breaking context by adding many messages.

## Reflection Questions

After completing these exercises, consider:

1. How specific should system prompts be?
2. When is `response_format` better than prompt-only formatting?
3. What happens when system prompts conflict with user prompts?
4. How many conversation turns can the AI remember?

## Next Steps

Ready to work with images? Move on to [Exercise 3: Vision](03_vision.md).

---

**Related Examples:**
- [bash/02_system_prompt_minimal.sh](../bash/02_system_prompt_minimal.sh)
- [bash/02_system_prompt_full.sh](../bash/02_system_prompt_full.sh)
- [nodejs/02_system_prompt.js](../nodejs/02_system_prompt.js)
- [python/02_system_prompt.py](../python/02_system_prompt.py)
- [go/02_system_prompt.go](../go/02_system_prompt.go)

[← Previous: Basic Chat](01_basic_chat.md) | [Back to Exercises](README.md) | [Next: Vision →](03_vision.md)
