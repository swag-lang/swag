#global testerror

#[Swag.Safety("analysis", true)]
func toto()
{
    var x: s32 = ?
    @assert(x == 0)
}