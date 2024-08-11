
#global testerror "Err0431"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
