var G = 0

func whatever(x: s32)
    #validifx @isconstexpr(x)
{
    G += 1
}

func whatever(x: s32)
    #validifx !@isconstexpr(x)
{
    G += 2
}

#test
{
    whatever(665)
    let x = 665
    whatever(x)
    @assert(G == 3)
}