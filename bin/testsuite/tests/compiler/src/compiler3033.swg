#global skip // no more relative pointers (18 july 2021)
//#[Swag.PrintBc]
#test
{
    var x: string~64 = "titi"
    @assert(@stringof(x) == "string~64")

    var y: [..]~64 s32
    @assert(@stringof(y) == "[..]~64 s32")
}