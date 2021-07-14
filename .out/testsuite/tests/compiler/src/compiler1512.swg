//#[Swag.PrintBc]
#test
{
    var x: u32 = 2
    var a = cast(bool) (x & 2)
    @assert(a == true)
}