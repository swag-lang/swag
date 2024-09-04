
#global testerror "Err0549"

#[Swag.Strict]
alias Alias = s32

func toto(x: Alias)
{
}

#test
{
    toto(0)
}