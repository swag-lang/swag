
#global testerror "Err0544"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
