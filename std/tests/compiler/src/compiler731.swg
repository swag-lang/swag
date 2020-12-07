#module "compiler731"

#unittest error
func(T) toto(x: *T)
{
	@print(t[0])
}
	
#test
{
	cpt := 50
	toto!s32(&cpt)
}
