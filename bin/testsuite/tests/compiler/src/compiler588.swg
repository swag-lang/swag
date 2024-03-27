


//#[Swag.PrintBc]
func(op: string) toto()
{
	/*#if op == "op"
		@print("op")
	#if op == "op1"
		@print("op1")*/
}

//#[Swag.PrintBc]
#test
{
	toto'("op")()
	toto'("op1")()
}