using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    arr.add("toto")
    visit v: arr
    {
        //Console.print("%\n", @typeof(v))
    }
}
