var t: struct
{
    inT: const [..] rune
}
 = {[1'rune, 2, 3, 4]}

#test
{
    @assert(@countof(t.inT) == 4)
    @assert(t.inT[0] == 1)
    @assert(t.inT[1] == 2)
    @assert(t.inT[2] == 3)
    @assert(t.inT[3] == 4)
}
