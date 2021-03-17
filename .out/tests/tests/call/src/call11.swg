using tests_foreign

using swag
var G: u64

#[swag.callback]
func entry(param: *void)
{
    ptr := cast(*u64) param
    G = ptr[1]
}

//#[swag.printbc, swag.optim(false)]
#test
{
    tcf_createThread(&entry, 666)
    @assert(G == 666)
}