using Core, Pixel

#test
{
    var str: RichString
    str.set("this is   a string")
    @assert(str.chunks.count == 4)
    @assert(str.chunks[0].slice == "this")
    @assert(str.chunks[1].slice == "is")
    @assert(str.chunks[2].slice == "a")
    @assert(str.chunks[3].slice == "string")
}