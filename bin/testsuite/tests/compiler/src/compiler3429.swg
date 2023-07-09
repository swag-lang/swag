#test
{
    var v: closure()->s32 = closure||()->s32
    {
        return 66
    }

    var x = v()
    @assert(x == 66)
}