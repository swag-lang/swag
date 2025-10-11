#test
{
    var x = 666'f32
    @init(x)(3.14) // Initialize to 3.14 instead of zero
    @assert(x == 3.14)
}
