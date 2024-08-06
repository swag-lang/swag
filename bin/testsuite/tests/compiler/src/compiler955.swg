
#global testerror "Err0523"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
