#global testerror "Saf0027"

#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
func toto(a: s16)
{
    let b = -1's16
    var c = a
    c /= b
    @print(c)
}

#test
{
    toto(Swag.S16.Min)
}
