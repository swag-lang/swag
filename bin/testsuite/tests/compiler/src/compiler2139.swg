//#global testerror

//#[Swag.Inline]
func toto(a: string = #os)
{
}

func toto1(a: string = #arch)
{
}

func toto3(a: string = #cfg)
{
}

#test
{
    toto()
}