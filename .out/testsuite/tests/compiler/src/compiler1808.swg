//#[Swag.PrintBc]
#test
{
    a := @[1's64, 2, 4]
    c := a[0..2]
    //@print(@typeof(c).name)
    @assert(c[0] == 1)
    @assert(@countof(c) == 3)

    c = a[1..2]
    @assert(c[0] == 2)
    @assert(@countof(c) == 2)
}