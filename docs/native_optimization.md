# My experience when doing native optimization by twiddling compiler flags

Can use:
- `-O3`: I have no issues with this flag now, but you don't too much magic, just use `-O1` or `-O2`

Avoid:
- `-ffast-math`: We doing pixel perfect rendering on CPU, fast-math will drop float precision, we cause misalign pixels

Required:
- `-fno-strict-aliasing`: I don't know much about compiler optimizer, but when removed this flag, I have misalign pixels, which cause ugly text.

Final advices:
- Don't touch theses landmine
