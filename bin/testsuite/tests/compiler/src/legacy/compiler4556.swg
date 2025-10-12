//#global testerror

alias A = string

#[Swag.Optimize("", false), Swag.Safety(.All, "", true)]
#test
{
    var a  = "toto"
    let x: &A = &a
    let y  = x[1 to 2]
    @assert(y == "ot")
}
