#import "core"
using core

//#[swag.printbc]
#test
{
    var str: String = "toto"
    u := unicode16.fromUtf8(str.toString())

    v := 0
    switch v
    {
        case 0:
            break;
    }
}