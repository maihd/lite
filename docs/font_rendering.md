# Font Rendering in Lite Editor

## Tricks
- Centering the `-` into `>`. See [lite_renderer_soft.c:331](/src/backends/lite_renderer_soft.c#391)

## Monospace Text Rendering
Tips, Tricks, Techniques to rendering monospace font.

Below tips are worked well on `stbtt_truetype.h`:
- 'k' is special case, they have padding, need doing manually padding. `k_glyph.x0 -= 1; k_glyph.x1 -= 1`
- 'M' or 'W' usually the most wided character, use its `xadvance` to detect monospace advance
- Use `ceilf` is better than `floorf` for pixel perfect rendering
- Usually, medium weight fonts have better visuals than regular weight fonts in monospace rendering

## Font choosing
- SauceCodePro: good operators, clear chars, easier to read, common choice for coding. Char height simple short, but acceptable.
- NotoSansMono: beautiful char, best font for text, I think. Support multiple languages by default. But operators is not good, not fit for coding.
