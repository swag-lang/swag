//#[Swag.PrintBc]
#test
{
    var x: const [..] s32
    @assert(x == null)
    x = [1, 2]
    @assert(x != null)
}