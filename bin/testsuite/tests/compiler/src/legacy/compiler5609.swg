#global testerror "Saf0027"

#[Swag.Safety("", "", true)]
#test
{
    let a = 100_000
    let b = 100_000
    let c = a * b
    @print(c)
}
