struct CStr
{
    buffer:     *u8
    length:     u32
}

var G: s64

impl CStr
{
    func opDrop(using self)
    {
        G += 1
    }
}

#[swag.inline]
func cstr(x: string)->CStr
{
    var tmp: CStr
    return tmp
}

//#[swag.printbc, swag.nooptim]
func toto()
{
    v := cstr("toto")
}

#test
{
    toto()
    @assert(G == 1)
}