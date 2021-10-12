//#global testerror

//#[Swag.Inline]
func toto(a: Swag.TargetOs = #os)
{
}

func toto1(a: Swag.TargetArch = #arch)
{
}

func toto3(a: string = #cfg)
{
}

#test
{
    toto()
}