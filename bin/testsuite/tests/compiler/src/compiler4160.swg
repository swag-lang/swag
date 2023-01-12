#global testerror

#[Swag.Safety("sanity", true)]
func toto()
{
    var x: s32 = ?
    @assert(x == 0)
}