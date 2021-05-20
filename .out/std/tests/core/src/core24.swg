
using core

#assert Latin1.isUpper(cast(u8) "A"'rune)
#assert !Latin1.isUpper(cast(u8) "a"'rune)
#assert Latin1.isLower(cast(u8) "a"'rune)