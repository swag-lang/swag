#global testerror "Saf0006"

alias A = string

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#[Swag.Safety("sanity", false)]
func toto(b: s32)
{
    var x: A
    var y = x[b..1]
}

#test
{
    toto(2)
}