#global testerror "Saf0027"

#[Swag.Safety("", true), Swag.Optimize("", false)]
func toto(a: s8)
{
    let b = -1's8
    var c = a
    c /= b
    @print(c)
}

#test
{
    toto(Swag.S8.Min)
}
