func toto() -> {s32, s32}
{
    return @{1, 2}
}

//#[Swag.printbc]
#test
{
    var x: {bool, bool} = @{true, false}
    @assert(x.item0 == true)
    @assert(x.item1 == false)
}