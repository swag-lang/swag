using Core, Pixel

#test
{
    var str: RichString
    str.set("this is   a string")
    str.compute()
    @assert(str.chunks.count == 7)
    @assert(str.chunks[0].slice == "this")
    @assert(str.chunks[1].isSpace)
    @assert(str.chunks[2].slice == "is")
    @assert(str.chunks[3].isSpace)
    @assert(str.chunks[4].slice == "a")
    @assert(str.chunks[5].isSpace)
    @assert(str.chunks[6].slice == "string")
}