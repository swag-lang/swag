#[Swag.Safety("sanity", false)]
func toto()->{a: rune, b: u32}
{
    const maskx = 0b00111111
    const mask2 = 0b00011111
    const mask3 = 0b00001111
    const mask4 = 0b00000111

    var dataof: ^u8
    let p0      = cast(rune) dref dataof

    let b1 = cast(u32) dataof[1]
    return {(p0 & mask2) << 6 | (b1 & maskx), 2}
}
