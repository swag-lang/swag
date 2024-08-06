#global testerror "Err0709"

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