using Core


#test
{
    var r: Parser.RegExp

    catch r.compile(#"(\D+)(123)"#)
    @assert(r.match("ABCDEF123"))
    @assert(r.getCapture(1) == "ABCDEF") 
    @assert(r.getCapture(2) == "123") 
}