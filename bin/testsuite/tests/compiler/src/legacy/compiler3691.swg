alias F = func()

func toto() {}

func titi(x: F) {}

#test
{
    titi(@mkcallback(&toto))
}
