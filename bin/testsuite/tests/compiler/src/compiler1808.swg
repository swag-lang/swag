//#[Swag.PrintBc]
#test
{
    var a = [1's64, 2, 4]
    var c = a[0 to 2]
    //@print(#typeof(c).name)
    @assert(c[0] == 1)
    @assert(@countof(c) == 3)

    c = a[1 to 2]
    @assert(c[0] == 2)
    @assert(@countof(c) == 2)
}
