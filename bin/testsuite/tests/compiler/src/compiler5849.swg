#global testerror "Err0561"

#[Swag.Macro]
func toto(x: #block bool)
{

}

#test
{
    toto(1.0)
}