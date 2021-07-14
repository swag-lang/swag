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

#[Swag.Optim("bytecode", true)]
func toto()
{
    v := cstr("toto")
}

#[Swag.Optim("bytecode", true)]
#test
{
    toto()
    @assert(G == 1)
}