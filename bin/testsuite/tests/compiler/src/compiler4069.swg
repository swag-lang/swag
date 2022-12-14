#test
{
    var toto: const [..] s32 = [10, 20]
    r := ref &toto
    x := 0
    loop r
    {
        x += 1
    }

    @assert(x == 2)
}