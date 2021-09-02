//#[Swag.PrintBc]
#test
{
    a := @[[1, 2, 3], [10, 20, 30]]
    @assert(a[0, 0] == 1)
    @assert(a[0, 1] == 2)
    @assert(a[0, 2] == 3)
    @assert(a[1, 0] == 10)
    @assert(a[1, 1] == 20)
    @assert(a[1, 2] == 30)
}