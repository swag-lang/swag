#global testerror "Saf0006"

alias A = string

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#[Swag.Sanity(false)]
func toto(b: s32)
{
    var x: A
    var y = x[b to 1]
}

#test
{
    toto(2)
}