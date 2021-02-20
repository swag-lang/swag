using swag

func toto(ptr: func() throw) throw
{
    assume ptr()
}

func toto1() throw
{
}

//#[swag.printbc]
#test
{
}