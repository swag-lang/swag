#global testerror "Err0575"

#[Swag.Overload]
func toto(x: string) {}

#[Swag.Overload]
func toto(x: string, y: string, disable: bool) {}

#test
{
    toto("titi", "tata", dis: true)
}