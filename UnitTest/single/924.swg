#module module924

//#[swag.printbc]
func average(x: any)
{
	tx:= cast(*string) @dataof(x)
	@print(tx[0])
}

#[swag.printbc]
#test
{
	average("toto")
	//average("titi")
}
