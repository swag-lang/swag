#module module596
#unittest pass semantic

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