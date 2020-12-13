
#unittest error

struct Color
{
    a: u32
    b: u32
}

impl Color
{
    func toto(using self, b: u32)
    {
        a = b
    }
}

#test
{
    var c: Color
    c.toto(0xFFFFFFFF)
    //@assert(c.a == 0xFFFFFFFF)
}