struct(T) A
{
}

var G: u32 = 0

impl A
{
    func opIndexAffect(using me, idx: u64, v: T)
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
    func toto(using me)
    {
        x[0] = null
    }
}
