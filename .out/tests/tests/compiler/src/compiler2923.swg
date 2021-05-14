#[swag.inline]
func consumeBits(width: u8)
{
    var codeBuffer: u32 = 1
    codeBuffer >>=,safe width
    @assert(codeBuffer == 1)
}

var G: u8 = 2
var G1: u32 = 0xFFFFFFFF

//#[swag.printbc]
#[swag.safety("", false)]
func toto()
{
    consumeBits(G & 1)
}

#test
{
    toto()
}