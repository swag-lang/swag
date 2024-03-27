using Core

#test
{
    const str = "⺁⺆⻇"

    var ptr   = @dataof(str)
    var count = @countof(str)
    var cpt   = 0
    loop
    {
        let sl = @mkslice(ptr, cast(u64) count)
        let (c, eat) = Core.Utf8.decodeRune(sl)
        if !eat do
            break
        ptr += eat
        count -= eat
        cpt += 1
    }

    @assert(@sizeof(str) != 3)
    @assert(cpt == 3)
}
