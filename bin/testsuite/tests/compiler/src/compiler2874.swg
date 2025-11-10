#[Swag.ExportType(.Methods)]
struct AA {}

impl AA
{
    func toto(x: s32)->s32
    {
        return x
    }
}

#test
{
    let t  = #typeof(AA)
    let f  = t.methods[0].value
    let fn = cast(func(s32)->s32) f
    @assert(fn(666) == 666)
}
