//
//#global testerror

alias A = [2] s32
var x: A = @[5, 10]

#test
{
    alias p = x
    @assert(@countof(p) == 2)

    alias tt = [..] s32
    alias tt1 = const *s32

    var v1: tt = @mkslice(&x[0], 2)
    var v2: tt1 = &v1[0]
    @assert(dref v2 == 5)
}