#global testerror "Err0460"

#[Swag.Safety(.All, .All, true)]
#test
{
    let a = Swag.S32.Min
    let b = a
    // Overflow: b cannot represent the positive equivalent of INT_MIN
    let c = -b
    @print(b)
}
