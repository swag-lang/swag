
using core

#assert latin1.isUpper(cast(u8) "A"'rune)
#assert !latin1.isUpper(cast(u8) "a"'rune)
#assert latin1.isLower(cast(u8) "a"'rune)