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

#[Swag.Inline]
func cstr(x: string)->CStr
{
    var tmp: CStr
    return tmp
}

#[Swag.optim("bytecode", true)]
func toto()
{
    v := cstr("toto")
}

#[Swag.optim("bytecode", true)]
#test
{
    toto()
    @assert(G == 1)
}