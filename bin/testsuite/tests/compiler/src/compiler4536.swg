#[Swag.Strict]
alias B = func(s32)->s32

#test
{
    let x: B = func(a) { return a; }
    let y  = x(1)
    @assert(y == 1)
}
