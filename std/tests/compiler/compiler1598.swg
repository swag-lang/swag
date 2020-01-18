var G: s32

func toto()->string
{
    return #"G = 666"#
}

#test
{
    #ast toto()
    @assert(G == 666)
}