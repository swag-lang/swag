using Swag

func toto()->s32 => 666

//#[Swag.PrintBc]
//#global testerror
#test
{
	var ptr = &toto
}
