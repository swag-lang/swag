/*//#global testerror

#[Swag.Strict]
alias B = closure(s32)

#test
{
    var x: B
    x = closure||(a) { @print(a); }
}*/

#test
{
    var fct: func(s32, s32)->bool
    fct = func(x, y) => x == y
}