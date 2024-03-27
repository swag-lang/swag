#test
{
    let v: closure()->s32 = closure||()->s32
    {
        return 66
    }

    let x = v()
    @assert(x == 66)
}