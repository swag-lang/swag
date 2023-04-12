#test
{
    a := ["1"'u8]
    ptr := &a[0]
    x := dref ptr
    @assert(x - "0"'u8 == 1)
}

#test
{
    a := ["1"'u16]
    ptr := &a[0]
    x := dref ptr
    @assert(x - "0"'u16 == 1)
}

/*
#test
{
    {
        var month = 11'u16
        var year = 2020'u16
        var day = 8'u16

        m := (month + 9) % 12
        y := year - m/10
        //@print("x: ", 365*y)
        dn := 365*y +,safe y/4 +,safe y/400 +,safe (m*306 + 5)/10 +,safe day-1 -,safe y/100
        dn += 3
        //@print(dn % 7)
    }

    {
        var month = 11's32
        var year = 2020's32
        var day = 8's32

        m := (month + 9) % 12
        y := year - m/10
        //@print("x: ", 365*y)
        dn := 365*y +,safe y/4 +,safe y/400 +,safe (m*306 + 5)/10 +,safe day-1 -,safe y/100
        dn += 3
        //@print(dn % 7)
    }
}
*/