#test
{
    var a = 10
    var b = 20
    var v: closure(s32) = closure|a,b|(x: s32)
    {
        @assert(a == 10)
        @assert(b == 20)
        @assert(x == 5)
    }

    v(5)
}
