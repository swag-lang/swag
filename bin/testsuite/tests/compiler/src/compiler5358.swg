#global testerror "Err0124"

#[Swag.Macro]
func toto()
{
    #up(0xFFF) a = 0
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