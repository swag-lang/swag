#test
{
    var x: { a: f32 }
    var y: { a: f32 }
    y.item0 = 100
    x       = y
    @assert(x.item0 == 100)
}
