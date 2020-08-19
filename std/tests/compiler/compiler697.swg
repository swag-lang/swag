#module "compiler697"


func(op: s32) toto()
{
}

#unittest error
#test
{
	c := 0
	toto!c();
}