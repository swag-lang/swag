using swag

func toto()->s32 => 666

//#[swag.printbc]
//#global testerror
#test
{
	ptr := &toto
}
