func(T, V) toto(x: T, y: V)
{
    @assert(@typeof(x) == @typeof(f32))
    @assert(@typeof(y) == @typeof(bool))
}

#test
{
    toto'(f32, bool)(1, true)
}