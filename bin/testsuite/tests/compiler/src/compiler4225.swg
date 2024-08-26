#global testerror "Err0724"

#[Swag.Mixin]
func toto()
{
    #ast
    {
        return "a = 0"
    }
}

//#[Swag.PrintBc]
#test
{
    toto()
}