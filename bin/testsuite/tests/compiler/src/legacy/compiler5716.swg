#global testerror "Saf0018"

alias A = string

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#[Swag.Sanity(true)]
#test
{
    var x: A
    var y = x[1 to 2]
}