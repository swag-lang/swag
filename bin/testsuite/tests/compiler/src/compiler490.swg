//#[Swag.PrintBc]
#test
{
    var res: { a: bool, b: bool } = {true, false}
    @assert(res.item0 == true)
    @assert(res.item1 == false)
}
