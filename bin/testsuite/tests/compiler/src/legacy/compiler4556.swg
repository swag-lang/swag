//#global testerror

alias A = string

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, true)]
#test
{
    var a  = "toto"
    let x: &A = &a
    let y  = x[1 to 2]
    @assert(y == "ot")
}
