#global testerror "Saf0002"
//#[Swag.PrintBc]
#test
{
    const array: [] any = [true, 2, 3.0, "4"]
    #assert cast(string) array[0] == true
}