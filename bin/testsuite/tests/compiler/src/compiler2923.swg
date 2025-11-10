#[Swag.Inline]
func consumeBits(width: u8)
{
    var codeBuffer: u32 = 1
    codeBuffer >>= width
    @assert(codeBuffer == 1)
}

var G:  u8 = 2
var G1: u32 = 0xFFFFFFFF

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, false)]
func toto()
{
    consumeBits(G & 1)
}

#test
{
    toto()
}
