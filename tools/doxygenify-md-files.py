#!/usr/bin/env python
import re
import sys

with open(sys.argv[1], 'r') as file:
    content = file.read()
    # Remove HTML comments when there are Doxygen instructions.
    content = re.sub('<\!-- doxygen:(.+)-->', '\g<1>', content, flags=re.S)
    # Replace `Note:` with `\note` so that Doxygen can generate a nice card.
    content = content.replace('Note:', '\\note')
    print(content)
