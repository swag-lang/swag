#test
{
    let a  = 10
    let b  = 20
    let v: closure(s32 )->s32 = closure|a, b|(v: s32)->s32
    {
        return a + b + v
    }

    let x = v(5)
    @assert(x == 35)
}
