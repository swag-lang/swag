
#global testerror "Err0541"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
