enum A
{
    R
}

#[Swag.Strict]
alias AL = A

//#[Swag.PrintBc]
#test
{
    var x:  AL = AL.R
    var x1: AL = R
}
