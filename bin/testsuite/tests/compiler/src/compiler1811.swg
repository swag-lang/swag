//#[Swag.PrintBc]
#test
{
    var a = [1, 2, 3, 4]
    var ptr = &a[0];

    var c = ptr[1..2]
    @assert(c[0] == 2)
    @assert(@countof(c) == 2)
}