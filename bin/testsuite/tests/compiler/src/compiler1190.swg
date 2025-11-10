//#global testerror "Err0004"

struct Color
{
    a:     u32
    b:     u32
}

impl Color
{
    func toto(me, b: u32)
    {
        me.a = b
    }
}

#test
{
    var c: Color
    c.toto(0xFFFFFFFF)
    //@assert(c.a == 0xFFFFFFFF)
}
