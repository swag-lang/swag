#global testerror "Saf0027"

#[Swag.Safety("", true)]
#test
{
    let a = Swag.S32.Max
    let b = 1
    let c = a + b
    @print(c)
}
