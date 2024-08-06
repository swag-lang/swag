#global testerror "Err0548"

#[Swag.Macro]
func toto()
{
    let a = #up(1000) b
}

#test
{
    toto()
}