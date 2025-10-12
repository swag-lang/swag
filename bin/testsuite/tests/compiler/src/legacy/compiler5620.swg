#global testerror "Saf0027"

#[Swag.Safety("", "", true), Swag.Optimize("", false)]
func toto(a: s8)
{
    let b = -1's8
    let c = a / b
    @print(c, " ")
}

#test
{
    toto(Swag.S8.Min)
}
