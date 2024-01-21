//#global skip
#global testerror "Err0400"

typealias A = [2] s32
var x: A = [5, 10]

#test
{
    typealias p = x
    @assert(@countof(p) == 2)

    typealias tt = [..] s32
    typealias tt1 = const *s32

    var v1: tt = @mkslice(&x[0], 2)
    let v2: tt1 = &v1[0]
    @assert(dref v2 == 5)
}