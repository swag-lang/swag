
#global testerror "Err0426"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
