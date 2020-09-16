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
}