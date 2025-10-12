#global testerror "Saf0007"

alias A = string

#[Swag.Optimize("", false), Swag.Safety(.All, "", true), Swag.Sanity(false)]
#test
{
    var x: A
    var y  = x[1 to 2]
}
