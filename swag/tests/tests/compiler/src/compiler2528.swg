//#global testerror

func toto()->s64
{
    @seterr("toto")
    return 555
}

#test
{
    y := 0
    x := toto() try
}