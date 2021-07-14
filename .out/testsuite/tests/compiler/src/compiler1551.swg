var G = 0

func toto(x: string)
{
    //@print(x)
    if x == "ABC"
        G += 1
}

//#[Swag.PrintBc]
#test
{
    G = 0

    var buffer: const [..] u8 = @[65, 66, 67]
    toto(cast(string) buffer)

    var buffer1: const [..] u8 = @[65, 66, 67]
    toto(cast(string) buffer1)

    var buffer2: [] u8 = @[65, 66, 67]
    toto(cast(string) buffer2)

    var buffer3: [3] u8 = @[65, 66, 67]
    toto(cast(string) buffer3)
    //@print(cast(s64) G)

    @assert(G == 4)
}