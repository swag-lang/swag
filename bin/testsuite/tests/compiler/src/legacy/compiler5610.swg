#global testerror "Saf0027"

#[Swag.Safety("", true)]
#test
{
    let a = Swag.S32.Min;
    let b = 1;
    let c = a - b;  // Underflow: c will wrap around to a large positive value
    @print(c)
}