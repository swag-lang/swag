struct AA
{
    p:   s64 = 55
}

#test
{
    let x = 10'u32
    let y = 5'u32
    var r = 0
    for (x - y) - 1 do
        r += 1
    @assert(r == 4)

    var xx: AA
    @assert(xx.p == 55)
}
