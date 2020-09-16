#[swag.inline]
func isDigit(c: u8) => "0"'u8 <= c <= "9"'u8

#test
{
    @assert(isDigit("0"'u8))
}
