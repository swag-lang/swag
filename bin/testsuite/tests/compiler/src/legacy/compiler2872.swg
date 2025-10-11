//#[Swag.PrintBc]
#test
{
    let x = 10
    let y = 5
    var c = 0
    for #reverse y to x
    {
        c += @index
    }
    @assert(c == 10 + 9 + 8 + 7 + 6 + 5)

    c = 0
    for y to x
    {
        c += @index
    }
    @assert(c == 10 + 9 + 8 + 7 + 6 + 5)
}
