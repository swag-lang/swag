func(T) toto(cb: closure(T, T))
{
    cb(10, 20)
}

#test
{
    let a = 10
    toto's32(closure|a|(x, y)
    {
        @assert(x == a)
        @assert(y == 20)
    }
)
}
