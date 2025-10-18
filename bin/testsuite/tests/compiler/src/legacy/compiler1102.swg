#global testerror "Err0476"

//#[Swag.EnumFlags]
enum Days
{
    A = 1
}

#test
{
    var x = ~Days.A
}
