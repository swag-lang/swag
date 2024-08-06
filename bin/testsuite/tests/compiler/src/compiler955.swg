
#global testerror "Err0545"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
