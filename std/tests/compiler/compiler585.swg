#module "compiler585"


#unittest error
func(op: string) toto()
{
}

#test
{
	c := 0
	toto!(c)()
}