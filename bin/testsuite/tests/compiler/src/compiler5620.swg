#global testerror "Saf0026"

#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#[Swag.Optim("", false)]
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
