
using core

#test
{
    enum RGB { Red; Green; Blue; }
    v := RGB.Green
    str := format.toString("%", v)
    //console.write("%\n", str)
    @assert(str == "RGB.Green")

    enum RGBS: string { Red = "A"; Green = "B"; Blue = "C"; }
    v1 := RGBS.Blue
    str = format.toString("%", v1)
    //console.write("%\n", str)
    @assert(str == "RGBS.Blue")

    #[swag.enumflags]
    enum Flags
    {
        Red = 1
        Green = 2
        Blue = 4
    }

    v2 := Flags.Red | Flags.Blue
    str = format.toString("%", v2)
    //console.write("%\n", str)
    @assert(str == "Flags.Red | Flags.Blue")
}