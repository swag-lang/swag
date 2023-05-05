#[Swag.Strict]
alias B = func(s32)->s32

#test
{
    var x: B = func(a) { return a; }
    y := x(1)
    @assert(y == 1)
}
