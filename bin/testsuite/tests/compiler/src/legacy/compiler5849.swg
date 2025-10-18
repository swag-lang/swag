#global testerror "Err0604"

#[Swag.Macro]
func toto(x: #code bool) {}

#test
{
    toto(1.0)
}
