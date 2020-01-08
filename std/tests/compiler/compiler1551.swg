var G = 0

func toto(x: string)
{
    if x == "ABC"
        G += 1
}

#test
{
    G = 0

    let buffer: const [..] u8 = [65, 66, 67]
    toto(buffer)
    let buffer1: [..] u8 = [65, 66, 67]
    toto(buffer1)    
    let buffer2: [] u8 = [65, 66, 67]
    toto(buffer2)    
    let buffer3: [3] u8 = [65, 66, 67]
    toto(buffer3)    

    @assert(G == 4)
}