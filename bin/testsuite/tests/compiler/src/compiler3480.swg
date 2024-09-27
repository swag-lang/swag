func(T) toto(cb: nn func||(T, T))
{
    cb(10, 20)
}

#test
{
    let a = 10
    toto's32(func|a|(x, y)
    {
        @assert(x == a)
        @assert(y == 20)
    })
}