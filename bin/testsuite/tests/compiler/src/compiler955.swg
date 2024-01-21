
#global testerror "Err0540"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
