
#global testerror "Err0442"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
