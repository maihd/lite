# Monospace Text Rendering
Tips, Tricks, Techniques to rendering monospace font.

Below tips are worked well on `stbtt_truetype.h`:
- 'k' is special case, they have padding, need doing manually padding. `k_glyph.x0 -= 1; k_glyph.x1 -= 1`
- 'M' or 'W' usually the most wided character, use its `xadvance` to detect monospace advance
- Use `ceilf` is better than `floorf` for pixel perfect rendering
- Usually, medium weight fonts have better visuals than regular weight fonts in monospace rendering
