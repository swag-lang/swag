//#[Swag.PrintBc]
#test
{
    var x: string~64 = "titi"
    @assert(@stringof(x) == "string~64")

    var y: [..]~64 s32
    @assert(@stringof(y) == "[..]~64 s32")
}