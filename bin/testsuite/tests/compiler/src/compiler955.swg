
#global testerror "Err0466"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
