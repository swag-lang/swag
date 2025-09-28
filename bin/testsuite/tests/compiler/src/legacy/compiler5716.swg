#global testerror "San0006"

alias A = string

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#[Swag.Sanity(true)]
#test
{
    var x: A
    var y = x[1 to 2]
}