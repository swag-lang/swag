#global testerror "Err0614"

#test
{
    const arr : [2, 2] u32 = [[10, 20], [30, 40]]

    //@print(cast(s64) #sizeof(arr))
    //@print(cast(s64) @countof(arr))

    //var cpt = 0
    foreach<Toto> xxx in arr
    {
        @print(cast(s64) xxx)
        //cpt += cast(s32) xxx
    }

    //@assert(cpt == 100)
}