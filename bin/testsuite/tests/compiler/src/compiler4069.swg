#test
{
    var toto: const [..] s32 = [10, 20]
    let r     = #ref &toto
    var x     = 0
    for r
    {
        x += 1
    }

    @assert(x == 2)
}
