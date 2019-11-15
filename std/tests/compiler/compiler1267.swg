#[swag.constexpr]
func isWhiteSpace(c: u8) => c == cast(u8) ' ' || (c >= cast(u8) '\x09' && c <= cast(u8) '\x0d') || c == cast(u8) '\xa0' || c == cast(u8) '\x85'
#assert isWhiteSpace(cast(u8) ' ')