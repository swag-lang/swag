#[Swag.PrintBc]
func(T) toto(cb: closure(T, T))
{
    cb(10, 20)
}

#test
{
    a := 10
    toto's32(@|a|(x, y)
    {
        @assert(x == a)
        @assert(y == 20)
    })
}