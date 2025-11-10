func toto(a: f32)
{
    @assert(a == 1.0)
}

#test
{
    //struct A {}
    //    var a: A
    var x = 0'u64
    x += 1
    toto(cast(f32) x)
}
