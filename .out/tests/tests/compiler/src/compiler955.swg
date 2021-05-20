
#global testerror

func toto()->s32 => 666

#[Swag.printbc]
#test
{
	ptr := toto
}
