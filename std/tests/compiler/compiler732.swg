

/*func(T) toto(x: T)
{
	var d: T = x
	var c: *T = &d
	@print(c[0])
}

#test
{
	cpt := "50"
	toto!string(cpt)
}*/

#test
{
	arr := @["toto", "titi"]
	var ptr: *string = &arr
	@assert(ptr[0] == "toto")
	@assert(ptr[1] == "titi")
}
