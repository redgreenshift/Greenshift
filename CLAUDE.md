## Bash paths
- **Always quote Windows paths** (with double quotes) in BASH commands unless you have a specific reason not to. The cost of quoting is near-zero; the cost of not quoting is the backslash issue.
- Common pitfall: `sed C:\path\file.txt` fails because `sed` interprets backslashes in its argument as escape sequences — the fix is `sed "C:\path\file.txt"`.
- Example: `Get-Content "C:\path\file.txt"` — always wrap the path in double quotes.

## Show Edit diffs to the user
- **Always show the Edit output in the terminal** so the user can see the diff. This is especially important when the edits are non-trivial (many lines, structural changes).
- **Bias toward Edit over Bash scripts** for file modifications. Show the Edit in the terminal with the diff so the user can understand the change. Only use a Bash/python script when:
  - Edit complains about duplicate matches and widening context doesn't help.
  - You need to check exact byte values (e.g., tabs, non-ASCII characters) before making an edit.
  - You need a bulk replacement across many locations.
- **If an Edit fails**, report it to the user and explain what you're going to do differently (e.g., "I'm going to use a Python script instead of Edit because the string matched multiple places").
- **Don't over-eagerly clean up** edits that "look slightly off." If the user's earlier edits look good but are in an unfamiliar format, prefer leaving them as-is rather than aggressively reforming.
