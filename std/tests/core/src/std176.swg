#import "core"
using core


//#[swag.printbc]
#test
{
    var str: String = "toto"
    cpt := 0
    visit str
    {
        cpt += 1
    }

    @assert(cpt == 4)
}
