#test
{
    const N           = 4
    const PowerOfTwo: [N] s32 = #run
    {
        var arr: [N] s32
        for i in arr do
            arr[i] = 1 << cast(u32) i
        return arr
    }

    #assert(PowerOfTwo[0] == 1)
    #assert(PowerOfTwo[1] == 2)
    #assert(PowerOfTwo[2] == 4)
    #assert(PowerOfTwo[3] == 8)
}
