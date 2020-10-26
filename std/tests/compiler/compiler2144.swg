struct(T) P
{
    next: *P
    next1: *P'T
}

#test
{
    var p: P's32
    @assert(@typeof(p.next) == *P's32)
    var p1: P'f64
    @assert(@typeof(p1.next) == *P'f64)
}