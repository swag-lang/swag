//#[Swag.PrintBc]
#test
{
    var a = [1, 2, 3, 4]
    var c = a[1..3]
    var d = c[1..2]
    @assert(d[0] == 3)
    @assert(@countof(d) == 2)
}