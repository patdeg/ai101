# Exercise 13: Reasoning with openai/gpt-oss-20b

[← Previous: Code Execution](12_code_execution.md) | [Back to Exercises](README.md) | [Next: Text-to-Speech →](14_text_to_speech.md)

## What You Learned

- Reasoning models show step-by-step thinking processes
- Three reasoning formats: raw, parsed, hidden
- Three effort levels: low, medium, high
- Cache optimization reduces costs by 50%
- Proper message ordering maximizes cache hits

## What is a Reasoning Model?

Reasoning models like `openai/gpt-oss-20b` are designed for complex problem-solving. Unlike chat models that go directly to an answer, reasoning models show their "thinking" process:

**Chat model:**
```
Q: How many 'r' letters in "strawberry"?
A: 3
```

**Reasoning model:**
```
Q: How many 'r' letters in "strawberry"?
<think>
Let me spell it out: s-t-r-a-w-b-e-r-r-y
Looking for 'r':
- Position 3: r (first r)
- Position 8: r (second r)
- Position 9: r (third r)
Total: 3
</think>
A: There are 3 'r' letters in "strawberry".
```

## Understanding Cache Optimization

### What is Prompt Caching?

Groq caches portions of your API requests to:
1. **Speed up responses** - Cached data doesn't need reprocessing
2. **Reduce costs** - Cached tokens cost 50% less

**For openai/gpt-oss-20b:**
- Normal input: $0.10 per 1M tokens
- Cached input: $0.05 per 1M tokens (50% discount!)
- Output: $0.50 per 1M tokens

### How Cache Works

Messages are processed in order. The cache works best when:
1. **Early messages are constant** - Same across many requests
2. **Later messages vary** - Change with each request

**❌ Bad ordering (variable first):**
```json
[
  {"role": "user", "content": "Does candidate have Python skills?"},  ← Changes each time
  {"role": "system", "content": "You are a recruiter..."},            ← Always same
  {"role": "user", "content": "Resume: John Doe, Skills: ..."}        ← Same per conversation
]
```
Cache hit rate: ~0% (first message always different)

**✅ Good ordering (constant first):**
```json
[
  {"role": "system", "content": "You are a recruiter..."},            ← Always same, CACHED
  {"role": "user", "content": "Resume: John Doe, Skills: ..."},       ← Same per convo, CACHED
  {"role": "user", "content": "Does candidate have Python skills?"}   ← Changes each time
]
```
Cache hit rate: ~70% (system + resume cached)

### Real-World Example: agentresume.ai

**Scenario:** Recruiter chatbot asking questions about candidate resumes

**Optimal message structure:**
```javascript
const messages = [
  // Level 1: Always constant (cached 100% of the time)
  {
    role: "system",
    content: "You are an AI recruiter assistant. Analyze candidate resumes and answer questions about their qualifications, experience, and skills."
  },

  // Level 2: Constant per conversation (cached during a session)
  {
    role: "user",
    content: `Candidate Resume:

Name: John Doe
Email: john@example.com
Skills: Python, Go, JavaScript, React, PostgreSQL
Experience:
- Senior Developer at TechCorp (2020-2024)
- Led team of 5 developers
- Built microservices architecture
...
    `
  },

  // Level 3: Changes each request (never cached)
  {
    role: "user",
    content: recruiterQuestion  // "Does this candidate have Python experience?"
  }
];
```

**Cost savings:**
- System prompt: ~100 tokens → Cached after first request
- Resume: ~500 tokens → Cached during entire conversation
- Questions: ~20 tokens each → Not cached

**Without caching:** 620 tokens × $0.10/1M = $0.000062 per request
**With caching:** (600 × $0.05 + 20 × $0.10) / 1M = $0.000032 per request
**Savings:** 48% cost reduction!

## Exercise 13.1: Reasoning Format Comparison

**Goal:** Understand when to use each reasoning format.

Test the same complex question with all three formats:

```bash
# Question: "A farmer has 17 sheep. All but 9 die. How many are left?"

# Format 1: Raw (default)
curl ... -d '{
  "model": "openai/gpt-oss-20b",
  "messages": [{"role": "user", "content": "A farmer has 17 sheep. All but 9 die. How many are left?"}],
  "reasoning_effort": "medium"
}'

# Format 2: Parsed
curl ... -d '{
  "model": "openai/gpt-oss-20b",
  "messages": [{"role": "user", "content": "A farmer has 17 sheep. All but 9 die. How many are left?"}],
  "reasoning_effort": "medium",
  "reasoning_format": "parsed"
}'

# Format 3: Hidden
curl ... -d '{
  "model": "openai/gpt-oss-20b",
  "messages": [{"role": "user", "content": "A farmer has 17 sheep. All but 9 die. How many are left?"}],
  "reasoning_effort": "medium",
  "reasoning_format": "hidden"
}'
```

**Compare:**
- Which format is easiest to read?
- Which format uses the most tokens?
- When would you use "hidden" format?

**Hint:** Hidden format is useful when you want the benefit of reasoning (better accuracy) without showing the thinking process to end users.

## Exercise 13.2: Reasoning Effort Levels

**Goal:** Understand the trade-off between reasoning depth and token cost.

Test with low, medium, high effort:

```javascript
const complexQuestion = "If you have a 3-gallon jug and a 5-gallon jug, how can you measure exactly 4 gallons?";

// Test each effort level
const efforts = ['low', 'medium', 'high'];

for (const effort of efforts) {
  const response = await callReasoning([
    { role: 'user', content: complexQuestion }
  ], 'raw', effort);

  console.log(`Effort: ${effort}`);
  console.log(`Tokens used: ${response.usage.total_tokens}`);
  console.log(`Response: ${response.choices[0].message.content}\n`);
}
```

**Analyze:**
- How many more tokens does "high" use vs "low"?
- Does higher effort produce better answers?
- When would you use "low" effort? "high" effort?

## Exercise 13.3: Cache Optimization Lab

**Goal:** Build a multi-turn conversation optimized for caching.

**Scenario:** Customer support chatbot with product documentation

```python
# Step 1: Define constant system context (always cached)
SYSTEM_PROMPT = """You are a helpful customer support agent for TechGadgets Inc.
Answer questions based on the product documentation provided."""

# Step 2: Load product docs (cached per product/session)
PRODUCT_DOCS = """
Product: SmartWidget Pro
Features:
- Battery life: 48 hours
- Waterproof: IP68 rating
- Connectivity: Bluetooth 5.0, WiFi 6
- Warranty: 2 years
...
"""

# Step 3: User questions (never cached - changes each time)
questions = [
    "What's the battery life?",
    "Is it waterproof?",
    "What's the warranty period?",
    "Does it support WiFi 6?"
]

# Optimal message structure
for question in questions:
    messages = [
        {"role": "system", "content": SYSTEM_PROMPT},      # Cached
        {"role": "user", "content": f"Docs:\n{PRODUCT_DOCS}"},  # Cached
        {"role": "user", "content": question}              # Not cached
    ]

    response = call_reasoning(messages)
    print(f"Q: {question}")
    print(f"A: {response['choices'][0]['message']['content']}\n")
```

**Experiment:**
1. Run with proper ordering (system → docs → question)
2. Run with bad ordering (question → docs → system)
3. Compare token costs

**Expected result:** First ordering saves ~50% on input tokens!

## Exercise 13.4: Multi-Step Reasoning Challenge

**Goal:** Use reasoning model for complex multi-step problems.

Test these progressively harder questions:

**Level 1: Logic puzzle**
```
"Three people check into a hotel room that costs $30. They each pay $10.
Later, the manager realizes the room is only $25 and gives $5 to the bellhop
to return. The bellhop keeps $2 and gives $1 back to each person.
Now each person has paid $9 (totaling $27), and the bellhop kept $2 (totaling $29).
Where is the missing dollar?"
```

**Level 2: Math problem**
```
"A snail is at the bottom of a 20-foot well. Each day it climbs up 3 feet,
but each night it slips back 2 feet. How many days will it take to reach the top?"
```

**Level 3: Code debugging**
```
"This Python function should return the factorial of n, but it's buggy.
Find and fix all errors:

def factorial(n):
    if n = 0:
        return 0
    else:
        return n * factorial(n)
"
```

**Use:**
- `reasoning_effort: "high"` for complex problems
- `reasoning_format: "parsed"` to see the thinking process
- Track token usage for each problem

## Exercise 13.5: Build a Cached Resume Analyzer

**Real-world application:** Implement the agentresume.ai pattern.

**Requirements:**
1. System prompt (constant)
2. Resume data (constant per conversation)
3. Multiple questions (variable)

**Implementation:**

```bash
#!/bin/bash
# resume_analyzer.sh

SYSTEM_PROMPT="You are an expert technical recruiter. Analyze resumes for technical skills, experience, and qualifications."

RESUME="
Name: Jane Smith
Skills: Python, Go, Docker, Kubernetes, AWS, React
Experience:
- Senior Backend Engineer at CloudCo (2021-2024)
  - Built microservices handling 1M+ requests/day
  - Led migration from monolith to microservices
  - Mentored 3 junior developers
- Backend Engineer at StartupXYZ (2019-2021)
  - Developed REST APIs with Python/Flask
  - Implemented CI/CD with GitHub Actions
Education:
- BS Computer Science, MIT (2019)
"

# Array of recruiter questions
questions=(
  "Does this candidate have Python experience?"
  "What's their experience with cloud platforms?"
  "Have they worked with microservices?"
  "Do they have leadership experience?"
  "What's their highest level of education?"
)

# Function to ask question with optimal caching
ask_question() {
  local question="$1"

  # Optimal ordering: system -> resume -> question
  curl -s -X POST "https://api.groq.com/openai/v1/chat/completions" \
    -H "Authorization: Bearer $GROQ_API_KEY" \
    -H "Content-Type: application/json" \
    -d @- <<EOF | jq -r '.choices[0].message.content'
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {"role": "system", "content": "$SYSTEM_PROMPT"},
    {"role": "user", "content": "Resume:\n$RESUME"},
    {"role": "user", "content": "$question"}
  ],
  "temperature": 0.6,
  "reasoning_effort": "low",
  "reasoning_format": "hidden"
}
EOF
}

# Ask all questions
for question in "${questions[@]}"; do
  echo "Q: $question"
  echo "A: $(ask_question "$question")"
  echo ""
done
```

**Enhancement ideas:**
1. Track cache hit rate across questions
2. Calculate cost savings from caching
3. Add conversation history (questions + answers cached)
4. Compare costs: cached vs non-cached ordering

## Exercise 13.6: Cost Tracking Dashboard

**Goal:** Build a token usage tracker that shows cache savings.

```python
class ReasoningTracker:
    def __init__(self):
        self.total_input_tokens = 0
        self.total_cached_tokens = 0
        self.total_output_tokens = 0
        self.requests = 0

    def track_request(self, response):
        usage = response['usage']
        self.total_input_tokens += usage['prompt_tokens']
        self.total_output_tokens += usage['completion_tokens']
        self.requests += 1

    def estimate_cached_tokens(self, system_tokens, context_tokens):
        """Estimate cached tokens (system + context after first request)"""
        if self.requests > 0:
            self.total_cached_tokens += (system_tokens + context_tokens)

    def print_report(self):
        # Calculate costs
        normal_input_cost = self.total_input_tokens * 0.10 / 1_000_000
        cached_benefit = self.total_cached_tokens * 0.05 / 1_000_000
        output_cost = self.total_output_tokens * 0.50 / 1_000_000

        total_without_cache = normal_input_cost + output_cost
        total_with_cache = (normal_input_cost - cached_benefit) + output_cost
        savings = total_without_cache - total_with_cache

        print("=" * 50)
        print("COST TRACKING REPORT")
        print("=" * 50)
        print(f"Total requests: {self.requests}")
        print(f"Input tokens: {self.total_input_tokens}")
        print(f"Cached tokens: {self.total_cached_tokens}")
        print(f"Output tokens: {self.total_output_tokens}")
        print("")
        print(f"Cost without caching: ${total_without_cache:.6f}")
        print(f"Cost with caching: ${total_with_cache:.6f}")
        print(f"Savings: ${savings:.6f} ({savings/total_without_cache*100:.1f}%)")
```

**Use it:**
```python
tracker = ReasoningTracker()

for i in range(10):
    response = call_reasoning(messages)
    tracker.track_request(response)
    tracker.estimate_cached_tokens(system_tokens=100, context_tokens=500)

tracker.print_report()
```

## Exercise 13.7: Reasoning vs Chat Comparison

**Goal:** Compare reasoning model with regular chat model.

Test both models on the same complex question:

```javascript
const question = "A bat and a ball cost $1.10 in total. The bat costs $1.00 more than the ball. How much does the ball cost?";

// Test 1: Regular chat model (llama-4-scout)
const chatResponse = await callGroq('meta-llama/llama-4-scout-17b-16e-instruct', [
  { role: 'user', content: question }
]);

// Test 2: Reasoning model (gpt-oss-20b)
const reasoningResponse = await callReasoning([
  { role: 'user', content: question }
], 'parsed', 'medium');

console.log("Chat model answer:", chatResponse.choices[0].message.content);
console.log("\nReasoning model:");
console.log("Thinking:", reasoningResponse.choices[0].message.reasoning);
console.log("Answer:", reasoningResponse.choices[0].message.content);
```

**Analyze:**
- Which model gets the correct answer? (Ball = $0.05, not $0.10!)
- How does showing reasoning help verify correctness?
- What's the cost difference between models?

## Reflection Questions

1. **Cache Strategy:** Why is message ordering so important for cost optimization?

2. **Reasoning Transparency:** When would you want to hide the reasoning process from end users?

3. **Effort Levels:** How would you automatically choose between low/medium/high reasoning effort?

4. **Production Use:** For the agentresume.ai scenario, how would you track which messages are likely to be cached vs not cached?

5. **Cost vs Quality:** At what point does the better accuracy of a reasoning model justify the 5x higher output cost ($0.50/1M vs $0.34/1M for llama-4-scout)?

## Real-World Applications

1. **Customer Support:** Cache product documentation and company policies
2. **Code Review:** Cache coding standards and style guides
3. **Legal Analysis:** Cache case law and regulations
4. **Medical Diagnosis:** Cache medical knowledge base
5. **Education:** Cache course materials and lesson plans

## Cost Optimization Checklist

✅ **Message ordering:** Constant messages first, variable last
✅ **System prompts:** Always place first in message array
✅ **Context data:** Place after system, before questions
✅ **Format choice:** Use "hidden" when reasoning isn't needed by users
✅ **Effort level:** Use "low" for simple questions
✅ **Tracking:** Monitor cache hit rates and savings

---

**Related Examples:**
- [bash/13_reasoning_minimal.sh](../bash/13_reasoning_minimal.sh)
- [bash/13_reasoning_full.sh](../bash/13_reasoning_full.sh)
- [nodejs/13_reasoning.js](../nodejs/13_reasoning.js)
- [python/13_reasoning.py](../python/13_reasoning.py)
- [go/13_reasoning.go](../go/13_reasoning.go)

**See also:**
- [Exercise 1: Basic Chat](01_basic_chat.md) - Token usage fundamentals
- [Exercise 12: Code Execution](12_code_execution.md) - Another openai model

[← Previous: Code Execution](12_code_execution.md) | [Back to Exercises](README.md) | [Next: Text-to-Speech →](14_text_to_speech.md)
