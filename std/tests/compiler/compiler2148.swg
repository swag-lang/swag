func toto(x: f32)=> 0'f32
func toto(x: f64)=> 0'f64
struct(T) A
{
    const X = toto(cast(T) 0)
}
#test
{
    var v: A'f32
    @assert(@typeof(v.X) == f32)
    var v1: A'f64
    @assert(@typeof(v1.X) == f64)
}