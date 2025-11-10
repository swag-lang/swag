//#[Swag.PrintBc]
#test
{
    let x = [{1, 2}, {3, 4}]
    //let x = {1,1}
    @assert(x[0].item0 == 1)
    @assert(x[0].item1 == 2)
    @assert(x[1].item0 == 3)
    @assert(x[1].item1 == 4)
}
