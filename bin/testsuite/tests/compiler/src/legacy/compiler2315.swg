
#global testerror "Err0561"

#[Swag.Strict]
alias Alias = s32

func toto(x: Alias)
{
}

#test
{
    toto(0)
}