#test
{
    var v: closure()->s32 = closure||()->s32
    {
        return 66
    }

    x := v()
    @assert(x == 66)
}