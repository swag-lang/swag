var G = 0

func toto(x: string)
{
    //@print(x)
    if x == "ABC" do
        G += 1
}

//#[Swag.PrintBc]
#test
{
    G = 0

    let buffer: const [..] u8 = [65, 66, 67]
    toto(cast(string) buffer)

    let buffer1: const [..] u8 = [65, 66, 67]
    toto(cast(string) buffer1)

    let buffer2: [?] u8 = [65, 66, 67]
    toto(cast(string) buffer2)

    let buffer3: [3] u8 = [65, 66, 67]
    toto(cast(string) buffer3)
    //@print(cast(s64) G)

    @assert(G == 4)
}
