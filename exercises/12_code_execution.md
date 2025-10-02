# Exercise 12: Code Execution (openai/gpt-oss-20b)

[← Previous: Web Search](11_web_search.md) | [Back to Exercises](README.md)

## What You’ll Learn
- Using a Python-execution-capable model (`openai/gpt-oss-20b`)
- Designing prompts that require code to be correct
- Reading executed code, output, and exceptions
- Mitigating runtime errors with try/except
- Comparing model variants (20B vs. 120B when available)

## Getting Started

Run a baseline example in your preferred language:
- Bash: `./bash/12_code_execution_full.sh`
- Node: `node nodejs/12_code_execution.js`
- Python: `python3 python/12_code_execution.py`
- Go: `go run go/12_code_execution.go`

Tip: Keep prompts short and specific. Ask for the exact code and output.

## Exercise 12.1: Verify by Execution (Math Challenge)

Goal: Use code execution to guarantee correctness.

Prompt:
“Compute the 2000th prime number using Python. Show the code you executed and the numeric result.”

Success criteria:
- You get a single number answer.
- The response includes the Python code used.

Reflect:
- Why is executing code more reliable than mental math for large values?

## Exercise 12.2: Trigger and Fix an Exception

Goal: Practice exception handling.

Prompt:
“Run Python: `print(1/0)` to show the exception. Then provide a fixed version that catches the error and prints a friendly message.”

Success criteria:
- You see the ZeroDivisionError in the executed output.
- The fixed code runs without crashing and prints a helpful message.

Reflect:
- How would you teach a beginner to read Python tracebacks?

## Exercise 12.3: Robust Parsing (Invalid JSON)

Goal: Build resilient code for real input messiness.

Prompt:
“Given a string that might not be valid JSON, write Python that tries to parse it with `json.loads`. If it fails, catch the exception and return a default dictionary with `{\"valid\": false, \"error\": \"...\"}`. Execute the code with both a valid and invalid input.”

Success criteria:
- Valid input produces a parsed object.
- Invalid input does not crash; it returns the default structure.

Reflect:
- Why is defensive programming important when handling user input?

## Exercise 12.4: Performance Contrast (Naive vs. Optimized)

Goal: Compare algorithm strategies using execution.

Prompt:
“Implement Fibonacci(35) two ways in Python: (1) naive recursion, (2) memoized or iterative. Execute both and show the elapsed times.”

Success criteria:
- You see measurable timing differences.
- The faster implementation is clearly identified.

Reflect:
- How do you decide when an optimization is worth the complexity?

## Exercise 12.5: Upgrade the Model

Goal: Explore model scaling effects (if available to your account).

Try the same prompt on:
- `openai/gpt-oss-20b`
- `openai/gpt-oss-120b`

Success criteria:
- You can describe differences (reasoning clarity, speed, reliability of code).
- You choose which model you’d ship for a classroom tool and justify why.

Reflect:
- Does bigger always mean better? Consider latency and cost.

## Stretch: Exception-Aware Mini Grader

Build a helper that accepts a student’s short Python snippet, executes it, and returns:
- stdout/stderr
- whether an exception occurred
- if so, a friendly message and tip on how to fix it

Safety tip: Scope to simple snippets and never run untrusted code in production without sandboxing.

## Related Examples
- [bash/12_code_execution_full.sh](../bash/12_code_execution_full.sh)
- [nodejs/12_code_execution.js](../nodejs/12_code_execution.js)
- [python/12_code_execution.py](../python/12_code_execution.py)
- [go/12_code_execution.go](../go/12_code_execution.go)

[← Previous: Web Search](11_web_search.md) | [Back to Exercises](README.md)
