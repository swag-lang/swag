#import "std"
using std.text, std.diagnostics

#test
{
    enum RGB { Red; Green; Blue; }
    v := RGB.Green
    str := format("%", v)
    //@print(cast(string) str)
    @assert(str == "Green")

    enum RGBS: string { Red = "A"; Green = "B"; Blue = "C"; }
    v1 := RGBS.Blue
    str = format("%", v1)
    //@print(cast(string) str)
    @assert(str == "Blue")

    #[swag.flags]
    enum Flags
    {
        Red = 1
        Green = 2
        Blue = 4
    }

    v2 := Flags.Red | Flags.Blue
    str = format("%", v2)
    @assert(str == "Red | Blue")
    //@print(cast(string) str)
}