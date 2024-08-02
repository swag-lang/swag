
#global testerror "Err0528"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
