//#[swag.printbc]
#test
{
    a := @[1, 2, 3, 4]
    ptr := &a[0];

    c := ptr[1..2]
    @assert(c[0] == 2)
    @assert(@countof(c) == 2)
}