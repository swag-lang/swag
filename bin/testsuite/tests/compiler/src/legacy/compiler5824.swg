#[Swag.NoInline]
//#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
func toto(a: *s32)->s32
{
    return dref a / 8
}

#test
{
    var x = -5
    @assert(toto(&x) == 0)
    @assert(-5 / 8 == 0)
}
