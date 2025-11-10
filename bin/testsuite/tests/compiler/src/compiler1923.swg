func toto(x: s32) => x + 1

alias AA = toto

#test
{
    let s = AA(66)
    @assert(s == 67)
}
