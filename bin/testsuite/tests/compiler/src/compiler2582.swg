using Swag

func toto(ptr: nn func() throw) throw
{
    assume ptr()
}

func toto1() throw
{
}

//#[Swag.PrintBc]
#test
{
}