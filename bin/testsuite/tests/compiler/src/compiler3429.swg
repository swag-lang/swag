#test
{
    var v: closure()->s32 = @||()->s32
    {
        return 66
    }

    x := v()
    @assert(x == 66)
}