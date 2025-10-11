struct A
{
    x:   const &s32 = undefined
}

#test
{
    var c = 42
    var a = A{&c}
    @assert(a.x == 42)
}
