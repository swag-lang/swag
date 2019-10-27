func toto()
{
	@print("toto")
}

#[swag.public]
func toto1() => toto()

#test
{
}