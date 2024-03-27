#global testerror "Saf0026"
#[Swag.Safety("", true)]
#test
{
    const month = 11'u16
    const year = 2020'u16
    const day = 8'u16

    const m = (month + 9) % 12
    const y = year - m/10
    @print("x: ", 365*y)
}
