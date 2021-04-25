#[swag.constexpr]
func isWhiteSpace(c: u8) => c == cast(u8) " "'rune || (c >= cast(u8) "\x09"'rune && c <= cast(u8) "\x0d"'rune) || c == "\xa0"'u8 || c == "\x85"'u8
#assert isWhiteSpace(cast(u8) " "'rune)