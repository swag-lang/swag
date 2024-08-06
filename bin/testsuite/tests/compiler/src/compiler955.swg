
#global testerror "Err0546"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
