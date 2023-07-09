func(T) toto() => 666'T

#test
{
    var x = toto's32()
    @assert(@typeof(x) == s32)
    var x1 = toto'f64()
    @assert(@typeof(x1) == f64)
}