//
//#global testerror

struct AA
{
    x: u8 = 5
}

//#[Swag.PrintBc]
#test
{
    var t: AA
    for t.x
    {
        if #index > 10:
            break;
    }
}
