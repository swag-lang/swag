//#[Swag.PrintBc]
func toto(x: any)
{
    v := cast(*s32) @dataof(x)
    @assert(v[0] == 1)
    @assert(v[1] == 2)
    @assert(v[2] == 3)
    @assert(v[3] == 4)
}

//#[Swag.PrintBc]
#test
{
    toto(@[1,2,3,4])
}