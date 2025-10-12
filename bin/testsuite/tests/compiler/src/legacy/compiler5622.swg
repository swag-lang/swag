#global testerror "Saf0027"

#[Swag.Safety("", "", true), Swag.Optimize("", false)]
func toto(a: s64)
{
    let b = -1
    var c = a
    c /= b
    @print(c)
}

#test
{
    toto(Swag.S64.Min)
}
