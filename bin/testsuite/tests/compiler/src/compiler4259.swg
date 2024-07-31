#global testerror "Err0145"

#[Swag.Macro]
func toto()
{
    #up(3) a = 0
}

#[Swag.Macro]
func toto1()
{
    toto()
}

#test
{
    var a = 0
    toto1()
}