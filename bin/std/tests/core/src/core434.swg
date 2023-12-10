using Core

#test
{
    var r: Parser.RegExp

    catch r.compile($"(\d+)-(\d+),(\d+)-(\d+)"$)
    @assert(r.match("99-5,6-777"))
    @assert(r.getCapture(1) == "99") 
    @assert(r.getCapture(2) == "5") 
    @assert(r.getCapture(3) == "6") 
    @assert(r.getCapture(4) == "777") 
}