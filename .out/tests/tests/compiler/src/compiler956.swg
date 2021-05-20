using Swag

func toto()->s32 => 666

//#[Swag.printbc]
//#global testerror
#test
{
	ptr := &toto
}
