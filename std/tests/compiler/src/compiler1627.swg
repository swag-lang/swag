struct(T) A
{
}

var G: u32 = 0

impl A
{
    func opIndexAffect(using self, idx: u32, v: T)
    {
        G += 1
    }
}

struct B
{
    x: A'(*u8)
}

impl B
{
    func toto(using self)
    {
        x[0] = null
    }
}
