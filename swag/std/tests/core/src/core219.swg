
using core

//#[swag.printbc]
#test
{
    var arr: Array'String
    arr.addAny("toto")
    visit v: arr
    {
        //console.print("%\n", @typeof(v))
    }
}