#module compiler597
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
	toto()
}