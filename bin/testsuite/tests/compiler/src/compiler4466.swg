#global testerror
#[Swag.Safety("", true)]
#test
{
    {
        var month = 11'u16
        var year = 2020'u16
        var day = 8'u16

        m := (month + 9) % 12
        y := year - m/10
        @print("x: ", 365*y)
        //@assert(365*y == 164040)
        dn := 365*y +,over y/4 +,over y/400 +,over (m*306 + 5)/10 +,over day-1 -,over y/100
        dn += 3
        //@print(dn % 7)
    }
}
