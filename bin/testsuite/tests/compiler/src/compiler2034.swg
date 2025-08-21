#global testerror "Err0024"

struct Argb
{
    a, r, g, b: u8
}

impl Argb
{
    func set(using me, a: u8)
    {
        me.a = a
    }
}