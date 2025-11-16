# AI Provider Comparison Guide

## Overview

This guide helps you choose the right AI provider for your use case. Example 01 (Basic Chat) includes implementations for all five providers across Bash, Node.js, Python, and Go.

## Quick Comparison Table

| Provider | Best For | Speed | Cost | Key Models | Context Window |
|----------|----------|-------|------|------------|----------------|
| **Groq** | Real-time apps | ⚡⚡⚡⚡⚡ | 💲 | Llama 4, Mixtral | 32K tokens |
| **OpenAI** | Advanced features | ⚡⚡⚡ | 💲💲💲 | GPT-5-nano, GPT-4o | 128K tokens |
| **Anthropic** | Careful analysis | ⚡⚡⚡ | 💲💲 | Claude Haiku/Sonnet/Opus | 200K tokens |
| **SambaNova** | Enterprise/Privacy | ⚡⚡⚡⚡ | 💲💲 | Llama 3.1 (8B-405B) | 128K tokens |
| **Demeterics** | Observability | (proxy) | + analytics | All Groq models | Via Groq |

## Detailed Provider Analysis

### Groq
**Strengths:**
- Fastest inference speed in the industry
- Cost-effective for high-volume applications
- Excellent for real-time use cases
- Strong Llama model support

**Weaknesses:**
- Limited model variety compared to OpenAI
- No proprietary models
- Smaller context windows

**Best Use Cases:**
- Chat applications requiring instant responses
- High-throughput batch processing
- Cost-sensitive deployments
- Real-time AI features

**Example Files:**
- `bash/01_basic_chat_minimal.sh`
- `nodejs/01_basic_chat.js`
- `python/01_basic_chat.py`
- `go/01_basic_chat.go`

### OpenAI
**Strengths:**
- Most advanced models (GPT series)
- Excellent function calling capabilities
- Strong vision and multimodal support
- Industry-leading performance on complex tasks
- Extensive documentation and community

**Weaknesses:**
- Higher cost per token
- Rate limits on lower tiers
- Potential for vendor lock-in

**Best Use Cases:**
- Complex reasoning tasks
- Code generation and analysis
- Function calling and tool use
- Creative content generation
- Research and development

**Example Files:**
- `bash/01_basic_chat_OPENAI_minimal.sh`
- `nodejs/01_basic_chat_OPENAI.js`
- `python/01_basic_chat_OPENAI.py`
- `go/01_basic_chat_OPENAI.go`

### Anthropic (Claude)
**Strengths:**
- Excellent at following complex instructions
- Very large context window (200K tokens)
- Nuanced, thoughtful responses
- Strong safety and alignment features
- Good at avoiding hallucinations

**Weaknesses:**
- Requires max_tokens parameter
- Different API format than OpenAI
- Fewer model options

**Best Use Cases:**
- Document analysis and summarization
- Complex instruction following
- Educational content
- Careful, analytical tasks
- Long-context applications

**Example Files:**
- `bash/01_basic_chat_ANTHROPIC_minimal.sh`
- `nodejs/01_basic_chat_ANTHROPIC.js`
- `python/01_basic_chat_ANTHROPIC.py`

### SambaNova
**Strengths:**
- Open-source models (no vendor lock-in)
- Enterprise-grade infrastructure
- OpenAI-compatible API format
- Privacy-focused (can be deployed on-premise)
- High-performance custom hardware

**Weaknesses:**
- Limited to open models
- Smaller ecosystem
- Less documentation

**Best Use Cases:**
- Enterprise deployments
- Privacy-sensitive applications
- Organizations preferring open models
- High-throughput production workloads

**Example Files:**
- `bash/01_basic_chat_SAMBA_minimal.sh`
- `nodejs/01_basic_chat_SAMBA.js`
- `python/01_basic_chat_SAMBA.py`

### Demeterics (Observability Proxy)
**Strengths:**
- Universal analytics across all providers
- Zero code changes for instrumentation
- Production monitoring and debugging
- Cost tracking and optimization
- Compliance audit trails
- A/B testing capabilities

**Weaknesses:**
- Adds slight latency (proxy overhead)
- Additional service dependency
- Requires separate API key

**Best Use Cases:**
- Production applications needing monitoring
- Multi-provider deployments
- Cost optimization projects
- Compliance and auditing requirements
- Performance analysis

**Example Files:**
- `bash/01_basic_chat_DEMETERICS_minimal.sh`
- `nodejs/01_basic_chat_DEMETERICS.js`
- `python/01_basic_chat_DEMETERICS.py`

## API Authentication Patterns

### Standard Bearer Token (Groq, OpenAI, SambaNova, Demeterics)
```
Authorization: Bearer YOUR_API_KEY
```

### X-API-Key Header (Anthropic)
```
x-api-key: YOUR_API_KEY
anthropic-version: 2023-06-01
```

### Combined Key Format (Demeterics Advanced)
```
Authorization: Bearer DEMETERICS_KEY;PROVIDER_KEY
```

## Cost Comparison (October 2025 Estimates)

| Provider | Model | Input (per 1M tokens) | Output (per 1M tokens) |
|----------|-------|----------------------|------------------------|
| **Groq** | Llama 4 Scout | $0.11 | $0.34 |
| **OpenAI** | GPT-5-nano | $0.50 | $1.50 |
| **OpenAI** | GPT-4o | $2.50 | $10.00 |
| **Anthropic** | Claude Haiku 4.5 | $0.25 | $1.25 |
| **Anthropic** | Claude Sonnet | $3.00 | $15.00 |
| **SambaNova** | Llama 3.1-8B | $0.10 | $0.10 |
| **SambaNova** | Llama 3.1-70B | $0.60 | $0.60 |

Note: Prices change frequently. Check provider websites for current pricing.

## Decision Framework

### Choose Groq when you need:
- ✅ Fastest possible inference
- ✅ Cost-effective scaling
- ✅ Real-time responses
- ✅ Simple to moderate complexity tasks

### Choose OpenAI when you need:
- ✅ Most advanced reasoning
- ✅ Function calling/tool use
- ✅ Cutting-edge features
- ✅ Extensive ecosystem support

### Choose Anthropic when you need:
- ✅ Very large context windows
- ✅ Careful, nuanced responses
- ✅ Strong instruction following
- ✅ Safety-first approach

### Choose SambaNova when you need:
- ✅ Open-source models
- ✅ Enterprise deployment
- ✅ Privacy guarantees
- ✅ No vendor lock-in

### Choose Demeterics when you need:
- ✅ Production observability
- ✅ Multi-provider analytics
- ✅ Cost tracking
- ✅ Compliance auditing

## Migration Guide

### From OpenAI to Others

**To Groq:**
- Change endpoint to `api.demeterics.com/groq/v1/chat/completions`
- Update model names (e.g., `meta-llama/llama-4-scout-17b-16e-instruct`)
- Same request format works

**To Anthropic:**
- Change endpoint to `api.anthropic.com/v1/messages`
- Add `max_tokens` (required)
- Change header to `x-api-key`
- Add `anthropic-version` header
- Parse `content[0].text` instead of `choices[0].message.content`

**To SambaNova:**
- Change endpoint to `api.sambanova.ai/v1/chat/completions`
- Update model names (e.g., `Meta-Llama-3.1-8B-Instruct`)
- Same request format works

## Testing Multiple Providers

Use the provided examples to test all providers with the same question:

```bash
# Set all API keys
export DEMETERICS_API_KEY="..."
export OPENAI_API_KEY="..."
export ANTHROPIC_API_KEY="..."
export SAMBANOVA_API_KEY="..."

# Run comparison
for provider in "" "_OPENAI" "_ANTHROPIC" "_SAMBA" "_DEMETERICS"; do
    case "$provider" in
      "") label="Demeterics Groq";;
      "_OPENAI") label="OpenAI";;
      "_ANTHROPIC") label="Anthropic";;
      "_SAMBA") label="SambaNova";;
      "_DEMETERICS") label="Demeterics (demo file)";;
    esac
    echo "Testing $label..."
    time ./bash/01_basic_chat${provider}_minimal.sh
    echo "---"
done
```

## Conclusion

There's no single "best" provider - the right choice depends on your specific needs:
- **Speed critical?** → Groq
- **Advanced features?** → OpenAI
- **Large context?** → Anthropic
- **Open models?** → SambaNova
- **Need observability?** → Add Demeterics proxy to any provider

The multi-vendor examples in this repository make it easy to experiment and find the best fit for your use case.
