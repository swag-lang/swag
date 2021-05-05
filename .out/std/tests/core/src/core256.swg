using core

#test
{
    const STR = "A\nBB\nCCC\n"
    arr := tokenize.splitLines(STR)
    //console.print(arr)
    @assert(arr.count == 3)
    @assert(arr[0] == "A")
    @assert(arr[1] == "BB")
    @assert(arr[2] == "CCC")
}