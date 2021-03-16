//#[swag.printbc]
#test
{
    a, b := @{1, 2}
    @assert(a.item0 == 1)
    @assert(a.item1 == 2)
    @assert(b.item0 == 1)
    @assert(b.item1 == 2)
}