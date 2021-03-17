
#global testerror

#test
{
    let arr : [2, 2] u32 = [[10, 20], [30, 40]]

    //@print(cast(s64) @sizeof(arr))
    //@print(cast(s64) @countof(arr))

    cpt := 0
    visit *xxx: 5
    {
        cpt += cast(s32) xxx
    }

    @assert(cpt == 100)
}