#test
{
    var arr : [2, 2] u32 = @[[10, 20], [30, 40]]

    //@print(cast(s64) @sizeof(arr))
    //@print(cast(s64) @countof(arr))

    cpt := 0
    cpt1 := 0'u32
    visit xxx, yyy: arr
    {
        cpt += cast(s32) xxx
        cpt1 += yyy
    }

    @assert(cpt == 100)
    @assert(cpt1 == 6)
}