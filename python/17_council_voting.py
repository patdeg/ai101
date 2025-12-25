#!/usr/bin/env python3
"""
Exercise 17: Council Voting - Multi-Persona Content Selection

Demonstrates:
- Demeterics Council API for content evaluation
- Multi-persona voting to select the best option
- Vote tallying and consensus metrics

Note: Uses Python standard library only
"""

import http.client
import json
import os
import sys

# Check for API keys
DEMETERICS_KEY = os.environ.get('DEMETERICS_API_KEY')
GROQ_KEY = os.environ.get('GROQ_API_KEY')

if not DEMETERICS_KEY or not GROQ_KEY:
    print('Error: Both DEMETERICS_API_KEY and GROQ_API_KEY must be set', file=sys.stderr)
    print('Get your Demeterics key: https://demeterics.ai', file=sys.stderr)
    print('Get your Groq key: https://console.groq.com', file=sys.stderr)
    sys.exit(1)

# Stories from December 3rd in History (simplified)
STORIES = """A) John Paul Jones raises the first American flag on a warship in 1775.
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
the bench, a watershed moment for American justice and civil rights."""


def call_council(question, content, num_personas=8):
    """Call the Demeterics Council API for voting evaluation."""
    conn = http.client.HTTPSConnection('api.demeterics.com')

    # Use dual-key format: demeterics_key;groq_key
    auth_header = f'{DEMETERICS_KEY};{GROQ_KEY}'

    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {auth_header}'
    }

    body = json.dumps({
        'question': question,
        'content': content,
        'num_personas': num_personas
    })

    conn.request('POST', '/council/v1/evaluate', body, headers)
    response = conn.getresponse()
    data = response.read().decode('utf-8')
    conn.close()

    return json.loads(data)


def display_results(result):
    """Display voting results in a readable format."""
    print('=' * 60)
    print('COUNCIL VOTING RESULTS')
    print('=' * 60)
    print()

    # Check for errors
    if 'error' in result:
        print(f"Error: {result['error'].get('message', 'Unknown error')}")
        return

    stats = result.get('stats', {})

    # Winner and consensus
    print(f"Winner: {stats.get('majority_vote', 'N/A')}")
    print(f"Consensus: {stats.get('vote_consensus', 'N/A')}")
    print()

    # Vote breakdown
    print('Vote Breakdown:')
    vote_breakdown = stats.get('vote_breakdown', {})
    for option, count in sorted(vote_breakdown.items(), key=lambda x: -x[1]):
        bar = '*' * count
        print(f"  {option}: {bar} ({count} votes)")
    print()

    # Overall score
    print(f"Average Interest Level: {stats.get('average_interested_level', 0):.1f}/100")
    print(f"Personas: {stats.get('successful_responses', 0)}/{stats.get('total_personas', 0)} responded")
    print()

    # Individual votes
    print('Individual Votes:')
    print('-' * 60)
    for persona in result.get('persona_responses', []):
        name = persona.get('name', 'Unknown')
        vote = persona.get('vote', 'N/A')
        reason = persona.get('vote_reason', 'No reason given')
        level = persona.get('interested_level', 0)
        print(f"  {name}: {vote}")
        print(f"    Reason: {reason}")
        print(f"    Interest: {level}/100")
        print()

    # Summary
    print('=' * 60)
    print('COUNCIL SUMMARY')
    print('=' * 60)
    print(result.get('summary', 'No summary available'))
    print()

    # Usage info
    usage = result.get('usage', {})
    if usage:
        print('=' * 60)
        print('USAGE')
        print('=' * 60)
        print(f"Total API calls: {usage.get('total_call_count', 0)}")
        print(f"Total tokens: {usage.get('total_tokens', 0)}")
        print(f"Cost: ${usage.get('total_cost_usd', 0):.4f}")


def main():
    print('=' * 60)
    print('Council Voting Demo - Best Story Selection')
    print('=' * 60)
    print()
    print('Question: Which story would you most want to watch as a short video?')
    print()
    print('Stories:')
    print(STORIES)
    print()
    print('Sending to Council API with 8 personas...')
    print()

    result = call_council(
        question='Which story would you most want to watch as a short video?',
        content=STORIES,
        num_personas=8
    )

    display_results(result)


if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        print(f'Error: {error}', file=sys.stderr)
        sys.exit(1)
