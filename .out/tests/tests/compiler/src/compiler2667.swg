#[Swag.macro]
func toto()
{
    a := 0
    switch a
    {
    case 0:
        b := 1
    case 1:
        b := 1
    }
}

#test
{
    toto()
}