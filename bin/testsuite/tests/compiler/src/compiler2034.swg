#global testerror "Err0024"

struct Argb
{
    a, r, g, b: u8
}

impl Argb
{
    func set(using self, a: u8)
    {
        self.a = a
    }
}