var G = 0
#test
{
    var c = 0'u8
    switch c
    {
    case 0'u8..9'u8:
        G += 1
    }

    @assert(G == 1)
}