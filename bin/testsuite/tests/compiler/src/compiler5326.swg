func titi() throw
{
    throw Swag.BaseError{"toto"}
}

//#[Swag.PrintBc]
func toto()
{
    assume titi()
}

#test
{
}
