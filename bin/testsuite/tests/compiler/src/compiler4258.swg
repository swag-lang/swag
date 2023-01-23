#[Swag.Macro]
func toto()
{
    `(2) a = 0
}

#[Swag.Macro]
func toto1()
{
    toto()
}

#test
{
    a := 0
    toto1()
}