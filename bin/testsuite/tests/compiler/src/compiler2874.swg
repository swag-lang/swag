#[Swag.ExportType("methods")]
struct AA
{

}

impl AA
{
    func toto(x: s32)->s32
    {
        return x
    }
}

#test
{
    var t = @typeof(AA)
    var f = t.methods[0].value
    var fn = cast(func(s32)->s32) f
    @assert(fn(666) == 666)
}