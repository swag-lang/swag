//#global testerror

#[Swag.Strict]
alias B = closure(s32)

#test
{
    var x: B
    x = closure||(a) { @print(a); }
}