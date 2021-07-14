using tests_foreign

using Swag
var G: u64

#[Swag.Callback]
func entry(param: *void)
{
    ptr := cast(*u64) param
    G = ptr[1]
}

//#[Swag.PrintBc, Swag.Optim(false)]
#test
{
    tcf_createThread(&entry, 666)
    @assert(G == 666)
}