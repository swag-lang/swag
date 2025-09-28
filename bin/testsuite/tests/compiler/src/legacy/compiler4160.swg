#global testerror "San0008"

#[Swag.Sanity(true)]
func toto()
{
    var x: s32 = undefined
    @assert(x == 0)
}