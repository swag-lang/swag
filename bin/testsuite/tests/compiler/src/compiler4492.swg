#test
{
    let x = @muladd(2'f32, 3, 4)
    @assert(x == 10)
}

#test
{
    let x = @muladd(2'f64, 3, 4)
    @assert(x == 10)
}
