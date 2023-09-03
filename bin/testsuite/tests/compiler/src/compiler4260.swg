#global testerror "Err0575"

#[Swag.Macro]
func toto()
{
    #up(0) a = 0
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