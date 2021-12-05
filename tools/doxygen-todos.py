#!/usr/bin/env python
import re
import sys

with open(sys.argv[1], 'r') as file:
    content = file.read()
    # Replace 3-line comments with a TODO. It isn't pretty but it works...
    content = re.sub(
        '//\s+TODO:(.+?)(\n\s*)(//.+)(\n\s*)(//.+)',
        '/// \\\\todo\g<1>\g<2>/\g<3>\g<4>/\g<5>',
        content,
        flags=re.S,
    )
    # Replace 2-line comments with a TODO.
    content = re.sub(
        '//\s+TODO:(.+?)(\n\s*)(//.+)',
        '/// \\\\todo\g<1>\g<2>/\g<3>',
        content,
        flags=re.S,
    )
    # Single-line TODOs.
    content = re.sub('//\s+TODO:', '/// \\\\todo', content)
    print(content, end='')
