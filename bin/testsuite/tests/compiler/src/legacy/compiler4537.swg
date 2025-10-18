#global testerror "Err0493"

#[Swag.Strict]
alias C = func(s32)

#test
{
    var y: C = func(a) { return a; }
}
