using Core

#test
{
    enum RGB
    {
        Red
        Green
        Blue
    }
    let v   = RGB.Green
    var str = Format.toString("%", v)
    //Console.write("%\n", str)
    @assert(str == "RGB.Green")

    enum RGBS: string
    {
        Red   = "A"
        Green = "B"
        Blue  = "C"
    }
    let v1 = RGBS.Blue
    str = Format.toString("%", v1)
    //Console.write("%\n", str)
    @assert(str == "RGBS.Blue")

    #[Swag.EnumFlags]
    enum Flags
    {
        Red   = 1
        Green = 2
        Blue  = 4
    }

    let v2 = Flags.Red | Flags.Blue
    str = Format.toString("%", v2)
    //Console.write("%\n", str)
    @assert(str == "Flags.Red | Flags.Blue")
}
