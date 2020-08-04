struct(T) Point
{
    x: T = 1.5
}

alias X = Point'f32

#test
{
    var pt: X
    @assert(pt.x == 1.5)
}