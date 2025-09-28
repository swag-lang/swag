#global testerror "Saf0002"
using Swag

#[Swag.Safety("", true)]
#[Swag.Sanity(false)]
#test
{
    var x: any
    let a = cast(s32) x
    @assert(a == 0)
}
