#test
{
    var a = 10
    var b = 20
    var v: closure(s32)->s32 = closure|a,b|(v: s32)->s32
    {
        return a + b + v
    }

    var x = v(5)
    @assert(x == 35)
}