#import "std"
using std

struct ST
{
    str: String
}

//#[swag.printbc]
#test
{
    var result: Array'ST
    var x: ST
    var y: [8] u8
    y[0] = "A"'u8
    y[1] = "B"'u8
    y[2] = 0
    dataof := cast(const *u8) y

    loop 20
    {
        x.str = cast(string) @mkslice(dataof, 2)
        result.add(x)
    }

    visit *c: result
        console.print("##%##\n", c.str)
}
