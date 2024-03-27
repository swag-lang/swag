#global testerror "San0008"

#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto(v: bool)
{
    var a, b: s32 = undefined
    if v do
        a = 0
    else do
        b = 0
    @assert(a == 0)
}