#global testerror "Err0594"
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