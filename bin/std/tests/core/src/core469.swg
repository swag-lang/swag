using Core

//#[Swag.PrintBc]
#test
{
    for 20
    {
        let x = Core.GUID64.getRandom()
        //Console.printf("%\n", x)
    }

    for 20
    {
        let x = Core.GUID128.getRandom()
        //Console.printf("%\n", x)
    }
}
