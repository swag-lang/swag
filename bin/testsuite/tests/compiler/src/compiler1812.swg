//#[Swag.PrintBc]
#test
{
    let a = [1, 2, 3, 4]
    let c = a[1 to 3]
    let d = c[1 to 2]
    @assert(d[0] == 3)
    @assert(@countof(d) == 2)
}
