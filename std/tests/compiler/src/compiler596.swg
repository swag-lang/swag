#module "compiler596"


#unittest error
func(op: string) toto()
{
}

func(op: s32) toto()
{
}

#test
{
	toto!(true)()
}