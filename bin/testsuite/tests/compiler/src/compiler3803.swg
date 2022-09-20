struct A
{
}

impl A
{
    #[Swag.Implicit, Swag.Inline]
    mtd opCast()->string
    {
        return ""
    }
}

//#[Swag.PrintBc]
func toto() throw
{
    var x: A
    throw x
}

#test
{
    catch toto()
}