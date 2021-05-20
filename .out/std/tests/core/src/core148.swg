
using core, Utf8, tokenize

//#[Swag.printbc]
#test
{
    var x: String = "This is a list of words, with: a bit of punctuation\tand a tab character."

    sep := @[" "'rune, ",", ".", ":", "\t"]
    arr := x.split(sep)

    visit v: arr
    {
        //@print(v)
        //@print("\n")
    }

    @assert(arr.count == 15)
    @assert(arr[0] == "This")
    @assert(arr[14] == "character")
}