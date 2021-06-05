var G: s32

func(op: string) toto(y: s32)
{
    #ast "G" ~op~ "y"
}

#test
{
    var x, y: s32
    toto'"="(10)
    @assert(G == 10)
    toto'"+="(5)
    @assert(G == 15)
}