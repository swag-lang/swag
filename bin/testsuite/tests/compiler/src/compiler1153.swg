#[Swag.Inline]
func toto(x, y: s64)
{
    @assert(x + y == 10)
}

//#[Swag.PrintBc]
#test
{
    toto(3, 7)
    toto(5, 5)

    let x = 9's64
    let y = 1's64
    toto(x, y)
}
