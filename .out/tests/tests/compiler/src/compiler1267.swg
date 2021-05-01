#[swag.constexpr]
func isWhiteSpace(c: u8) => c == cast(u8) " "'rune or (c >= cast(u8) "\x09"'rune and c <= cast(u8) "\x0d"'rune) or c == "\xa0"'u8 or c == "\x85"'u8
#assert isWhiteSpace(cast(u8) " "'rune)