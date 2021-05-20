
using core

//#[Swag.printbc]
#test
{
    var arr: Array'String
    arr.addAny("toto")
    visit v: arr
    {
        //console.print("%\n", @typeof(v))
    }
}