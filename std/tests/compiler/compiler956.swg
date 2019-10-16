using swag

func toto()->s32 => 666

//#[swag.printbc]
//#unittest error
#test
{
	ptr := &toto
}
