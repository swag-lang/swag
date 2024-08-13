
#global testerror "Err0433"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
