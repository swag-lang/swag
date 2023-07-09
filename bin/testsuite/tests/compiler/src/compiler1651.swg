//#[Swag.PrintBc]
func toto1(x: any)
{
    var v = cast(s32) x
    @assert(v == 100)
}

#run
{
    toto1(100)
}