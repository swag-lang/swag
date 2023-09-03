#global testerror "Err0873"

#[Swag.Safety("sanity", true)]
func toto()
{
    let x: s32 = undefined
    @assert(x == 0)
}