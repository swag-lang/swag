//#[Swag.PrintBc]
#test
{
    var x1: [?] typeinfo = [s32, #typeof(#type { a: s32, b: bool })]
    @assert(x1[1] == #typeof(#type { a: s32, b: bool }))
}
