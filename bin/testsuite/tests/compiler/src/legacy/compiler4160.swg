#global testerror "Saf0034"

#[Swag.Sanity(true)]
func toto()
{
    var x: s32 = undefined
    @assert(x == 0)
}