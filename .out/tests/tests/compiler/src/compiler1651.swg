//#[Swag.printbc]
func toto1(x: any)
{
    v := cast(s32) x
    @assert(v == 100)
}

#run
{
    toto1(100)
}