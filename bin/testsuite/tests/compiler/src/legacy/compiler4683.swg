#global testerror "Err0234"
using Swag

#[Swag.Safety(.All, .All, true)]
#test
{
    let b = "XYZ"
    @assert(b[3] == 0)
}
