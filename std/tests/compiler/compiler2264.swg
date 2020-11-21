#test
{
    var x: struct{v: f32;}
    var y: struct{v1: f32;}
    y.v1 = 100
    x = y
    @assert(x.v == 100)
}