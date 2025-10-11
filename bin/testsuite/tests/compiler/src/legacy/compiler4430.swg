//#global skip
using Swag

var G = 0

#test
{
    var x = [1, 2]
    //@print(#typeof(x).name)
    var ptr = &x
    let y   = #ref &ptr
    //@print(#typeof(y).name)
    var z = y[0]
}
