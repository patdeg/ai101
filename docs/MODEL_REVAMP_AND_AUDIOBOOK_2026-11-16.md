# Model revamp, audiobook series and local Whisper (2026-11-16)

A record of what changed, what was measured, and what is still unverified.

## 1. New lessons

| Lesson | File | What it teaches |
|---|---|---|
| 8b | `python/08b_whisper_local.py` | Local speech-to-text with `faster-whisper` (the only Python example that needs `pip install`) |
| 19 | `python/19_audiobook_voices.py` | Gemini TTS: narrator plus a two-speaker dialogue |
| 20 | `python/20_audiobook_sfx.py` | ElevenLabs sound effects, caching, a hard call budget |
| 21 | `python/21_audiobook_music.py` | AI writes a JSON score; we validate it, write MIDI, render with a small synthesizer |
| 22 | `python/22_audiobook_mix.py` | Timeline mix: music, narration, SFX, dialogue, SFX, music |

Shared story and cast: `python/audiobook/episode.json`. Outputs: `python/audiobook/out/` (gitignored).
Each lesson has a matching file in `exercises/`; `exercises/16_podcast.md` gained exercises 8-10.

All audio is 24 kHz, 16-bit, mono so stems from different providers mix without resampling.

## 2. Model changes (checked against OpenRouter's live model list)

| Use | Before | After |
|---|---|---|
| Chat, system prompt, templates, tool use, AI composer | `openai/gpt-oss-20b` | `qwen/qwen3.8-flash` ($0.15 / $0.47 per 1M) |
| Vision | `google/gemma-4-31b-it` | `google/gemini-3.8-flash` ($0.83 / $4.13 per 1M) |
| Web search (`:online`) | `openai/gpt-oss-20b:online` | `google/gemini-3.8-flash:online` (about $0.03 per request) |
| Anthropic vendor example | `claude-haiku-4-5` | `claude-haiku-5-5` |

Unchanged on purpose: examples 13 and 14 (Groq-direct `openai/gpt-oss-20b`), `meta-llama/llama-guard-4-12b`,
`openai/gpt-oss-safeguard-20b`, `openai/whisper-1`, Fish Audio TTS.

## 3. Findings worth keeping

1. **Qwen3.8 Flash is a reasoning model.** Hidden thinking tokens count against `max_tokens`. Example 02
   (`max_tokens` 150) returned `None` until the request carried `"reasoning": {"enabled": false}`.
   Examples 01 and 02 in all languages, and lesson 21, now send it. Example 11 and 03 have large budgets and work without it.
2. **`:online` is model-dependent.** On `qwen/qwen3.8-flash` the search plugin silently did nothing (cost about
   $0.00005, no citations). On `google/gemini-3.8-flash` it ran (about $0.03, current answer). Always check the
   request cost or citations before trusting `:online`.
3. **Old prices had drifted.** `gemma-4-31b-it` was $0.20 / $0.40, not the documented $0.09 / $0.34.
4. **Never trust AI output.** Lesson 21 validates and clamps every field, catches malformed or empty answers,
   falls back to a built-in score, and only caches an answer after it passes validation.
5. **MIDI ordering bug (fixed).** At equal ticks a note-off must sort before a note-on, or a repeated note is cut off.
6. **Local Whisper measured:** `tiny.en` on an arm64 Linux CPU, 11.2 s clip in 4.4 s (real-time factor 0.39).
   It mishears "on this continent" as "on its continent", which exercise 8b uses.

## 4. Review

An independent review (Opus 5.5, read-only, no paid API calls) found stale prices in about 15 files, an
example 12 slug that still used Qwen with `:online`, lesson 21 crash paths, the MIDI ordering bug,
low `max_tokens` in the C and C++ template examples, wrong exercise navigation, and stale prose.
All were fixed.

## 5. Not verified (open items)

- **Lessons 19 and 20 have never run against the live Gemini and ElevenLabs APIs.** The model id
  `gemini-3.8-flash-tts` (override with `GEMINI_TTS_MODEL`), the request shape, and ElevenLabs
  `output_format=pcm_24000` are written from documentation and memory. Both lessons stop on the first error.
- The C, C++ and Arduino edits were read, not compiled (`cJSON.h`, `json.hpp` not installed on the test host).
- Gemini 3.8 Flash may spend part of the vision and web-search token budget on thinking; untested.
- Lesson 19 does not check for a blocked or empty TTS response; lesson 22 does not duck music under voices.
- The OpenAI and SambaNova vendor-comparison model names were not changed (catalogs not verifiable here).
- `gofmt` reports formatting drift in several Go files; this predates these changes.
- Whisper-1, Fish Audio and OpenAI TTS slugs cannot be checked through OpenRouter's `/models` list (no audio models).
