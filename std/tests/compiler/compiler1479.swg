#test
{
    let arr : [2, 2] u32 = [[10, 20], [30, 40]]
    
    //cpt := 0
    loop xxx: arr
    {
        @print(cast(s64) xxx)
        //cpt += cast(s32) xxx
    }

    //@assert(cpt == 100)
}