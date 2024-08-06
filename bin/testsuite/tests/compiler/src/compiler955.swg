
#global testerror "Err0481"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
