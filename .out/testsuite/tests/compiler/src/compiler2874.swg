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
    t := @typeof(AA)
    f := t.methods[0].value
    fn := cast(func(s32)->s32) f
    @assert(fn(666) == 666)
}