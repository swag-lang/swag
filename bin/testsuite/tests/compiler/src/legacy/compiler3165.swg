#global skip
#global testerror

#[Swag.Safety("", "", true)]
func toto(x: cvarargs)
{
    @cvastart(cast(*Swag.CVaList) null)
}

#test
{
    toto(1)
}