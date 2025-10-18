#global testerror "Err0604"

#[Swag.Strict]
alias Alias = s32

func toto(x: Alias) {}

#test
{
    toto(0)
}
