#test
{
    var x: {f32}
    var y: {f32}
    y.item0 = 100
    x = y
    @assert(x.item0 == 100)
}