//#[Swag.PrintBc]
#test
{
    let arr: const [..] u32 = [10, 20, 30, 40]
    var cpt  = 0
    foreach xxx in arr
    {
        //@print(cast(s64) xxx)
        cpt += cast(s32) xxx
    }

    @assert(cpt == 100)
}
