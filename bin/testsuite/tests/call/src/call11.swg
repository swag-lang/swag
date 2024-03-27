using test_foreign

using Swag
var G: u64

func entry(param: ^void)->u32
{
    var ptr = cast(^u64) param
    G = ptr[1]
    return 0
}

//#[Swag.PrintBc, Swag.Optim(false)]
#test
{
    tcf_createThread(&entry, 666)
    @assert(G == 666)
}