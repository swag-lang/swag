#global testerror "Saf0028"
using Swag

#[Swag.Safety("", true)]
#[Swag.Safety("sanity", false)]
#test
{
    var x: any
    let a = cast(s32) x
    @assert(a == 0)
}
