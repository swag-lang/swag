struct AA
{
    x, y: f64
    struct BB { x, y: string = "true" }

    z:   BB
}

#test
{
    var v: AA
    @assert(v.z.x == "true")
    @assert(v.z.y == "true")
    var ptr = #typeof(AA.BB)
    @assert(#sizeof(v) == 6 * 8)
}
