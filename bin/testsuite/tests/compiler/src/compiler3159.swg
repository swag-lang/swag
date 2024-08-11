#global testerror "Err0474"
func printf(format: cstring, args: cvarargs)
{
    printf1(format, args)
}

func printf1(format: cstring, args: cvarargs)
{
}

#test
{
    printf("toto")
}