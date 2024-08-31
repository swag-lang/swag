#global testerror "Err0320"

#[Swag.Safety("", true)]
#test
{
    let a = Swag.S32.Min;
    let b = a;  // Overflow: b cannot represent the positive equivalent of INT_MIN
    let c = -b
    @print(b)
}