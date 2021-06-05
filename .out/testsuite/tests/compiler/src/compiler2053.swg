//#[Swag.printbc]
func toto(x: any)
{
    v := dref cast(**s32) @dataof(x)
    @assert(v[0] == 1)
    @assert(v[1] == 2)
    @assert(v[2] == 3)
    @assert(v[3] == 4)
}

//#[Swag.printbc]
#test
{
    var arr: [] s32 = @[1,2,3,4]
    toto(&arr)
}