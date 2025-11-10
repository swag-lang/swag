/*func(T) toto(x: T)
{
	var d: T = x
	var c: *T = &d
	@print(c[0])
}

#test
{
	var cpt = "50"
	toto!string(cpt)
}*/

#test
{
    var arr  = ["toto", "titi"]
    let ptr: [*] string = &arr
    @assert(ptr[0] == "toto")
    @assert(ptr[1] == "titi")
}
