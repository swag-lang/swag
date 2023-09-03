#global testerror "Err0808"
func(T) toto(x: s32, y: T)
{
}

//#[Swag.PrintBc]
#test
{
	toto(0's32, 1)
}