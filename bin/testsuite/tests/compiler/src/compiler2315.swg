
#global testerror "Err0605"

#[Swag.Strict]
alias Alias = s32

func toto(x: Alias)
{
}

#test
{
    toto(0)
}