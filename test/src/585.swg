#module module585
#unittest pass semantic

#unittest error
func(op: string) toto()
{
}

#test
{
	c := 0
	toto!(c)()
}