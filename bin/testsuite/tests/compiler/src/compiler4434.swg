func toto(a: &s32, b: &&s32)
{
    a = b
    if a != b do
        @assert(false)
}

#test
{
    var a = 1's32
    var b = 2's32
    toto(&a, moveref &b)
    @assert(a == 2)
}
