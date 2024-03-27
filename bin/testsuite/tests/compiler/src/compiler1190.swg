#global testerror "Err0017"

struct Color
{
    a:     u32
    b:     u32
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
}
