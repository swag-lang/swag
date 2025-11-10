struct C
{
    length: [4] s32
}

var g_RD: [40] C

enum EE { R, G }
alias E = EE

#test
{
    @assert(@countof(C.length) == 4)
    @assert(@countof(EE) == 2)
    @assert(@countof(E) == 2)
    @assert(@countof([1, 2, 3]) == 3)
}
