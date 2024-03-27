var G = 0
#test
{
    let c = 0'u16
    switch c
    {
    case 0'u16 to 9'u16:
        G += 1
    }

    @assert(G == 1)
}
