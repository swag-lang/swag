//
//#unittest error

struct AA
{
    x: u8 = 5
}

//#[swag.printbc]
#test
{
    var t: AA
    loop t.x
    {
        if @index > 10
            break;
    }
}
