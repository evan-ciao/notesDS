import re
with open("convertTwo.txt", 'r') as file:
    for line in file:
        result = re.sub(r'(?<![$$])\{|\}(?![$$])', '', line).strip()
        print(result)