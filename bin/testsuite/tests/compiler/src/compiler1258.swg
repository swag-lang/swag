func toto()->{ a: f32, b: bool }
{
    return {666, true}
}

//#[Swag.PrintBc]
#test
{
    let (x, y) = toto()
    @assert(x == 666)
    @assert(y == true)
}
