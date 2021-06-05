using Swag

func toto(ptr: func() throw) throw
{
    assume ptr()
}

func toto1() throw
{
}

//#[Swag.printbc]
#test
{
}