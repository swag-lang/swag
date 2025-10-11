struct A
{
    b:     bool = true
    c:     s32 = 666
}

alias AA = A
var g: [4] AA

#test
{
    for i in @countof(g)
    {
        @assert(g[i].b)
        @assert(g[i].c == 666)
    }
}
