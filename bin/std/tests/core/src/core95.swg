using Core

//#[Swag.PrintBc]
#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "%", false)
    @assert(buf.toString() == "false")

    buf.clear()
    Format.append(&buf, "%", true)
    @assert(buf.toString() == "true")

    buf.clear()
    Format.append(&buf, "%", '1''rune)
    @assert(buf.toString() == "1")

    //format(&buf, "%", &buf)

    //var str = buf.toString()
    //Console.write(cast(string) str)
}
