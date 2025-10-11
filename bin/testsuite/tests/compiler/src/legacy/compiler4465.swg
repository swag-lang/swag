#test
{
    var a   = ['1''u8]
    let ptr = &a[0]
    let x   = dref ptr
    @assert(x - '0''u8 == 1)
}

#test
{
    var a   = ['1''u16]
    let ptr = &a[0]
    let x   = dref ptr
    @assert(x - '0''u16 == 1)
}

/*
#test
{
    {
        var month = 11'u16
        var year = 2020'u16
        var day = 8'u16

        var m = (month + 9) % 12
        var y = year - m/10
        //@print("x: ", 365*y)
        var dn = 365*y + #wrap y/4 + #wrap y/400 + #wrap (m*306 + 5)/10 + #wrap day-1 - #wrap y/100
        dn += 3
        //@print(dn % 7)
    }

    {
        var month = 11's32
        var year = 2020's32
        var day = 8's32

        var m = (month + 9) % 12
        var y = year - m/10
        //@print("x: ", 365*y)
        var dn = 365*y + #wrap y/4 + #wrap y/400 + #wrap (m*306 + 5)/10 + #wrap day-1 - #wrap y/100
        dn += 3
        //@print(dn % 7)
    }
}
*/
