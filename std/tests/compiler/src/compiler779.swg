#module "compiler779"

#unittest error
func toto(x: const *u32)
{
	x = null
}

#test
{
}