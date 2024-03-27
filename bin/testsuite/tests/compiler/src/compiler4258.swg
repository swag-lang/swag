#[Swag.Macro]
func toto()
{
    #up(2) a = 0
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
