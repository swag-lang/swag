
#global testerror

func toto()->s32 => 666

#[swag.printbc]
#test
{
	ptr := toto
}
