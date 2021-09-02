var G: s32

#test
{
    #ast
    {
        return @"G = 666"@
    }

    @assert(G == 666)
}