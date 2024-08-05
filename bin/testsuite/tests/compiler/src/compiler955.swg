
#global testerror "Err0534"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
