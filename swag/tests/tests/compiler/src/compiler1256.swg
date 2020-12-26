
#unittest error

func toto() -> {f32, bool}
{
	return {666, true}
}

#test
{
	(x, x) := toto()
}