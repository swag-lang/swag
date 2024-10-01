#global testerror "San0006"

alias A = string

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#[Swag.Sanity(true)]
func toto(b: s32)
{
    var x: A
    var y = x[b..1]
}

#test
{
    toto(2)
}