
#unittest error

#[swag.strict]
alias T = s32

func toto(x: T)
{
}

#test
{
    toto(0)
}