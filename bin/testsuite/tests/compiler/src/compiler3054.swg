//#[Swag.PrintBc]
#test
{
    var x1: [] typeinfo = @[s32, @typeof(#type {s32, bool})]
    @assert(x1[1] == @typeof(#type {s32, bool}))
}