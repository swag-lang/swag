
using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    arr.addAny("toto")
    visit v: arr
    {
        //Console.print("%\n", @typeof(v))
    }
}