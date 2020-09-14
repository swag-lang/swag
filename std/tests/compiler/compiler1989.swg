func(T) swap(x, y: *T)
{
    var tmp = move deref x
    x[0] = nodrop move deref y
    y[0] = nodrop move tmp
}

#test
{
    var x: u32 = 666
    var y: u32 = 777
    swap(&x, &y)
    @assert(x == 777)
    @assert(y == 666)
}