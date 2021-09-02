//#[Swag.PrintBc]
func toto(x: ...)
{
    v := cast(*s32) @dataof(x[0])
    @assert(v[0] == 1)
    @assert(v[1] == 2)
    @assert(v[2] == 3)
    @assert(v[3] == 4)
}

//#[Swag.PrintBc]
#test
{
    arr := @[1,2,3,4]
    toto(arr)
}