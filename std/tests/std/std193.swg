#import "std"
using std

#test
{
    {
        res := String.from("как поживаешь")
        res.makeUpper(CharacterSet.Unicode)
        @assert(res == "КАК ПОЖИВАЕШЬ")
        //console.print("%", res)
    }

    {
        res := String.from("КАК ПОЖИВАЕШЬ")
        res.makeLower(CharacterSet.Unicode)
        @assert(res == "как поживаешь")
        //console.print("%", res)
    }
}