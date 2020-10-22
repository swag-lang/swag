#import "std"
using std

//#[swag.printbc]
#test
{
    var x: String = "This is a list of words, with: a bit of punctuation\tand a tab character."

    var sep: const [..] char = @[" "'char, ","'char, "."'char, ":"'char, "\t"'char]
    arr := x.split(sep)

    loop arr
    {
        //@print(arr[@index])
        //@print("\n")
    }

    @assert(arr.count == 15)
    @assert(arr[0] == "This")
    @assert(arr[14] == "character")
}