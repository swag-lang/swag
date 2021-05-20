func(T) inc(x: *T)
{
    dref x += 1
}

//#[Swag.printbc]
func(T) map(cb: func(*T))
{
    var x: T
    cb(&x)
    @assert(x == 1)
}

#test
{
    map's32(&inc)
}