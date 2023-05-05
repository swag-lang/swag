#global testerror

enum A : string
{
    R = ""
}

#[Swag.Strict]
alias AL = A

//#[Swag.PrintBc]
#test
{
    var x: AL
}