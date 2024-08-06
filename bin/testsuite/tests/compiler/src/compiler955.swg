
#global testerror "Err0543"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
