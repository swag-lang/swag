#module module590
#unittest pass semantic

//#[swag.printbc]
func(op: string) toto()
{
	if(op == "toto")
	{
	}
}

//#[swag.printbc]
#test
{
	toto!("toto")()
}