
#global testerror "Err0490"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
