struct A
{
    x:   &s32 = undefined
}

#test
{
    var c = 42
    var a = A{&c}
    @assert(a.x == 42)
    c = 44
    @assert(a.x == 44)
    a.x = 46
    @assert(c == 46)
}
