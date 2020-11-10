struct CStr
{
    length: u32
}

impl CStr
{
    //#[swag.printbc]
    func opDrop(self)
    {
    }

    func opPostMove(self)
    {
    }
}

#[swag.inline]
func cstr(x: string)->CStr
{
    var tmp: CStr
    tmp.length = 666
    return tmp
}

//#[swag.printbc]
//#[swag.nooptim]
#test
{
    v := cstr("toto")
    @assert(v.length == 666)
}