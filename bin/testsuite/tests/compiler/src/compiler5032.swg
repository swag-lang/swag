#global testerror "Saf0026"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
func toto(x: s8)
{
    let a = -x
    @print(a)
}

#test
{
    toto(-128)
}