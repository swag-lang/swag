//#global skip
using Swag

var G = 0

#test
{
    var x = func()
    {
        G = 1
    }

    let r = #ref &x
    r()
    @assert(G == 1)
}
