
#global testerror "Err0434"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
