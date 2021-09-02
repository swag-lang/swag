var G = 0

func whatever(x: s32)
    #checkif @isconstexpr(x)
{
    G += 1
}

func whatever(x: s32)
    #checkif !@isconstexpr(x)
{
    G += 2
}

#test
{
    whatever(665)
    x := 665
    whatever(x)
    @assert(G == 3)
}