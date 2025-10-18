#global testerror "Err0737"

//#[Swag.EnumFlags]
enum Days
{
    A = 1
}

#test
{
    var x = ~Days.A
}
