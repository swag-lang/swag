
#global testerror "Err0473"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
