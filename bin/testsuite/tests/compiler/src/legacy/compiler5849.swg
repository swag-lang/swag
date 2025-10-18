#global testerror "Err0745"

#[Swag.Macro]
func toto(x: #code bool) {}

#test
{
    toto(1.0)
}
