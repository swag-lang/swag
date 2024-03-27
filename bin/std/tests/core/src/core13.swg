using Core

//#[Swag.PrintBc]
#test
{
    let x = Math.min(1's32, 2's32)
    @assert(x == 1)
}
