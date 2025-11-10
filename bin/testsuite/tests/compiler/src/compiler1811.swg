//#[Swag.PrintBc]
#test
{
    var a   = [1, 2, 3, 4]
    let ptr = &a[0]

    let c = ptr[1 to 2]
    @assert(c[0] == 2)
    @assert(@countof(c) == 2)
}
