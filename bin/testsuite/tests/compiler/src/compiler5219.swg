//#[Swag.PrintBc]
func toto(x: ref const [..] s32)
{
    x = [3, 4]
}

//#[Swag.PrintBc]
#test
{
    var a: [..] s32
    toto(&a)
    @assert(a[0] == 3)
    @assert(a[1] == 4)
    @assert(@countof(a)==2)
}