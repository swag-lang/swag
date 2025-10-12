struct CStr
{
    buffer:     *u8
    length:     u32
}

var G: s64

impl CStr
{
    func opDrop(me)
    {
        G += 1
    }
}

#[Swag.Inline]
func cstr(x: string)->CStr
{
    var tmp: CStr
    return tmp
}

#[Swag.Optimize(.ByteCode, true)]
func toto()
{
    var v = cstr("toto")
}

#[Swag.Optimize(.ByteCode, true)]
#test
{
    toto()
    @assert(G == 1)
}
