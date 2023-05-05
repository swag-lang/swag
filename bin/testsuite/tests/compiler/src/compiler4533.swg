//#global testerror

#[Swag.Strict]
alias B = closure(s32)

#test
{
    var x: B = closure||(a: s32) { @print(a); }
}