func titi()
{
    @print("titi\n")
}

func toto()
{
    @print("toto\n")
    titi()
}

#test
{
    @dbgbreak()
    toto()
}