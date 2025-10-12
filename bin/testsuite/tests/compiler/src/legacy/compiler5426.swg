var g = 55

struct A
{
    a:   string
}

impl A
{
    mtd const opIndex(x: u64)->const &s32
    {
        return &g
    }
}

func toto(x: u64)->const &s32
{
    return &g
}

#[Swag.Optimize(.All, false)]
#test
{
    var arr: A
    let (a) = {arr[0]}
    @assert(a == 55)
}
