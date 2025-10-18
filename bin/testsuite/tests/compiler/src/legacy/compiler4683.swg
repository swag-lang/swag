#global testerror "Err0289"
using Swag

#[Swag.Safety(.All, .All, true)]
#test
{
    let b = "XYZ"
    @assert(b[3] == 0)
}
