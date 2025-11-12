func(T) toto(cb: func||(T, T))
{
    cb(10, 20)
}

#test
{
    toto's32(func(x, y)
    {
        @assert(x == 10)
        @assert(y == 20)
    } )
}
