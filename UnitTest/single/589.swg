#module module589
//#unittest pass semantic

//#[swag.printbc]
func(op: string) toto()
{
}

//#[swag.printbc]
#test
{
	toto!("op")()
	toto!("op1")()
}