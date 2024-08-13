#global testerror "Err0493"

#[Swag.Macro]
func toto()
{
    let a = #up(1000) b
}

#test
{
    toto()
}