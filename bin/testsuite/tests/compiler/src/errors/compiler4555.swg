#global testerror "Saf0007"

alias A = string

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, true), Swag.Sanity(false)]
#test
{
    var x: A
    var y  = x[1 to 2]
}
