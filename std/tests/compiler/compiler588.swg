


//#[swag.printbc]
func(op: string) toto()
{
	/*#if op == "op"
		@print("op")
	#if op == "op1"
		@print("op1")*/
}

//#[swag.printbc]
#test
{
	toto'("op")()
	toto'("op1")()
}