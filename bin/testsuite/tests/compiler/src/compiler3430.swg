#test
{
    a := 10
    b := 20
    var v: closure(s32)->s32 = @|a,b|(v: s32)->s32
    {
        return a + b + v
    }

    x := v(5)
    @assert(x == 35)
}