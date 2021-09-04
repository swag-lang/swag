using Core

#test
{
    var r: Parser.RegExp
    assume r.compile("ab")
    @print(r.match("ab"))
}