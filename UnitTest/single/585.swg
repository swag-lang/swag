#module module585
#unittest pass semantic

//#unittest error
func(op: string) toto()
{
}

#test
{
	toto!("op")()
}