#module compiler595
#unittest pass semantic

#unittest error
func(op: string) toto()
{
}

#test
{
	toto!(0)()
}