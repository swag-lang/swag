//
//#global testerror

struct AA
{
    x: u8 = 5
}

//#[Swag.printbc]
#test
{
    var t: AA
    loop t.x
    {
        if @index > 10
            break;
    }
}
