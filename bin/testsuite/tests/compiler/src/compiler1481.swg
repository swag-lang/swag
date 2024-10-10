#global testerror "Err0136"

#test
{
    const arr : [2, 2] u32 = [[10, 20], [30, 40]]

    //@print(cast(s64) #sizeof(arr))
    //@print(cast(s64) @countof(arr))

    var cpt = 0
    foreach &xxx in 5
    {
        cpt += cast(s32) xxx
    }

    @assert(cpt == 100)
}