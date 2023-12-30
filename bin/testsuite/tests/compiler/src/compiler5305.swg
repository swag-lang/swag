func titi() throw
{
}

//#[Swag.PrintBc]
func toto()
{
    assume
    {
        titi()
    }
}

#test
{
    toto()
}