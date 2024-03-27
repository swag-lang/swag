//#global testerror
func(N: s32) toto(v: f32)->[N]f32
{
    var result: retval
    loop i: N do
        result[i] = v*v
    return result
}

#test
{
    var v = toto'5(2)
    @assert(@countof(v) == 5)
    @assert(v[0] == 4)
    @assert(v[4] == 4)

    var v1 = toto'10(3)
    @assert(@countof(v1) == 10)
    @assert(v1[0] == 9)
    @assert(v1[9] == 9)
}