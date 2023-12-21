using Core
#test
{
    var r: Parser.RegExp
    //assume r.compile("((1[5678][0-9])|(19[0-3]))cm|(59|6[0-9]|7[0-6])in")
    assume r.compile("(6[0-9]|7[0-6])in")
    @assert(r.match("74in"))
}
