func toto()->{ a, b: s32 }
{
    return {1, 2}
}

//#[Swag.PrintBc]
#test
{
    var x: { a, b: bool } = {true, false}
    @assert(x.item0 == true)
    @assert(x.item1 == false)
}
