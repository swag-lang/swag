struct CStr
{
    length: u32
}

impl CStr
{
    //#[Swag.PrintBc]
    func opDrop(me) {}

    func opPostMove(me) {}
}

#[Swag.Inline]
func cstr(x: string)->CStr
{
    var tmp: CStr
    tmp.length = 666
    return tmp
}

//#[Swag.PrintBc]
//#[Swag.nooptim]
#test
{
    let v = cstr("toto")
    @assert(v.length == 666)
}
