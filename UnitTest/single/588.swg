#module module588
#unittest pass semantic

var x = 1

#[swag.printbc]
func(op: string) toto()
{
}

#[swag.printbc]
#test
{
	toto!("op")()
	toto!("op1")()
}