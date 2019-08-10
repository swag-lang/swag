#module module586
#unittest pass semantic

func(op: string) toto()
{
	//@print("coucou")
}

#test
{
	toto!("op")()
}