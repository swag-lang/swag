func(T) toto() => 666'T

#test
{
    x := toto's32()
    @assert(@typeof(x) == s32)
    x1 := toto'f64()
    @assert(@typeof(x1) == f64)
}