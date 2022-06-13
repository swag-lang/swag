alias F = func()

#[Swag.Callback]
func toto()
{
}

func titi(x: F)
{
}

#test
{
    titi(@mkcallback(&toto))
}