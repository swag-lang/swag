var G = 0
#test
{
    let c = 0'u8
    switch c
    {
    case 0'u8 to 9'u8:
        G += 1
    }

    @assert(G == 1)
}
