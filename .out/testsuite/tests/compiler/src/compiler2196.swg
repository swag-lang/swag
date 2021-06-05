struct CStr
{
    length: u32
}

impl CStr
{
    //#[Swag.printbc]
    func opDrop(self)
    {
    }

    func opPostMove(self)
    {
    }
}

#[Swag.inline]
func cstr(x: string)->CStr
{
    var tmp: CStr
    tmp.length = 666
    return tmp
}

//#[Swag.printbc]
//#[Swag.nooptim]
#test
{
    v := cstr("toto")
    @assert(v.length == 666)
}