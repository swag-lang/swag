var G = 0
#test
{
    c := 0'u16
    switch c
    {
    case 0'u16..9'u16:
        G += 1
    }

    @assert(G == 1)
}