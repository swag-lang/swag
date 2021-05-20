#[Swag.compiler]
func toto()->bool
{
    return true
}

#test
{
    #assert #run toto()
}