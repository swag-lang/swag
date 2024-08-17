#global testerror "Saf0026"

#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func toto(a: s64)
{
    let b = -1
    let c = a / b
    @print(c)
}

#test
{
    toto(Swag.S64.Min)
}
