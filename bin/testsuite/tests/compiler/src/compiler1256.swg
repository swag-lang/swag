#global testerror "Err0626"

func toto() -> {a:f32, b:bool}
{
	return {666, true}
}

#test
{
	var (x, x) = toto()
}