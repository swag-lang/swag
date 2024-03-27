using Core

#test
{
    const STR = "A\nBB\nCCC\n"
    var arr = Tokenize.splitLines(STR)
    //Console.print(arr)
    @assert(arr.count == 3)
    @assert(arr[0] == "A")
    @assert(arr[1] == "BB")
    @assert(arr[2] == "CCC")
}