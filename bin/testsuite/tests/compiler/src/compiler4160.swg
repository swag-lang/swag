#global testerror "San0008"

#[Swag.Safety("sanity", true)]
func toto()
{
    var x: s32 = undefined
    @assert(x == 0)
}