#global testerror

func toto() -> {a:f32, b:bool}
{
	return {666, true}
}

#test
{
	(x, x) := toto()
}