#global testerror "Err0627"

func toto() -> {a:f32, b:bool}
{
	return {666, true}
}

#test
{
	var (x, x) = toto()
}