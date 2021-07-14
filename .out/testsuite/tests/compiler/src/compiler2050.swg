//#[Swag.PrintBc]
func toto(x: ...)
{
    v := cast(*s32) @dataof(x[0])
    @assert(v[0] == 1)
    @assert(v[1] == 2)
    @assert(v[2] == 3)
    @assert(v[3] == 4)
}

/*func toto(x: const [4] s32)
{
    @print(cast(s64) x[0])
}
*/

//#[Swag.PrintBc]
#test
{
    toto(@[1,2,3,4])
}