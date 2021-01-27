#[swag.constexpr]
func isWhiteSpace(c: u8) => c == cast(u8) " "'char || (c >= cast(u8) "\x09"'char && c <= cast(u8) "\x0d"'char) || c == "\xa0"'u8 || c == "\x85"'u8
#assert isWhiteSpace(cast(u8) " "'char)