//#[Swag.PrintBc]
#test
{
    a := @[ [ [1, 2], [1, 2] ], [ [1, 2], [1, 2] ] ]
    @assert(a[0, 0, 0] == 1)
    @assert(a[0, 0, 1] == 2)
}