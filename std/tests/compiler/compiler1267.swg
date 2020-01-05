#[swag.constexpr]
func isWhiteSpace(c: u8) => c == cast(u8) " "'char || (c >= cast(u8) "\x09"'char && c <= cast(u8) "\x0d"'char) || c == cast(u8) "\xa0"'char || c == cast(u8) "\x85"'char
#assert isWhiteSpace(cast(u8) " "'char)