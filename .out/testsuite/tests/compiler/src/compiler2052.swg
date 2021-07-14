//#[Swag.PrintBc]
func toto(x: any)
{
    v := cast([..] s32) x
    @assert(v[0] == 1)
    @assert(v[1] == 2)
    @assert(v[2] == 3)
    @assert(v[3] == 4)
}

//#[Swag.PrintBc]
#test
{
    var arr: [] s32 = @[1,2,3,4]
    var x: [..] s32 = arr
    toto(x)
}