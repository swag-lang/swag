
#global testerror "Err0428"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
	var ptr = toto
}
