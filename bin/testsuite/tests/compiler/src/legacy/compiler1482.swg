#test
{
    var arr: [2, 2] u32 = [[10, 20], [30, 40]]

    //@print(cast(s64) #sizeof(arr))
    //@print(cast(s64) @countof(arr))

    var cpt = 0
    foreach &xxx in arr
    {
        cpt += cast(s32) dref xxx
    }

    @assert(cpt == 100)
}
