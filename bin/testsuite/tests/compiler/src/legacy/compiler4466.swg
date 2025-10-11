#global testerror "Saf0027"
#[Swag.Safety("", true)]
#test
{
    {
        let month = 11'u16
        let year  = 2020'u16
        let day   = 8'u16

        let m = (month + 9) % 12
        let y = year - m / 10
        @print("x: ", 365 * y)
        //@assert(365*y == 164040)
        var dn = 365 * y + #wrap y / 4 + #wrap y / 400 + #wrap (m * 306 + 5) / 10 + #wrap day - 1 - #wrap y / 100
        dn += 3
        //@print(dn % 7)
    }
}
