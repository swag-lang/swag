#test
{
    let v: func||()->s32 = func||()->s32
    {
        return 66
    }

    let x = v()
    @assert(x == 66)
}
