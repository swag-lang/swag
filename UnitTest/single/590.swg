#module module590
//#unittest pass semantic

#[swag.printbc]
func(op: string) toto()
{
	if(op == "toto")
		@print("toto")
	if(op == "toto1")
		@print("toto1")		
}

//#[swag.printbc]
#test
{
	toto!("toto")()
	toto!("toto1")()
}