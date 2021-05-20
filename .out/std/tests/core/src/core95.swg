
using core

//#[Swag.printbc]
#test
{
    var buf: ConcatBuffer
    format.append(&buf, "%", false)
    @assert(buf.toString() == "false")

    buf.clear()
    format.append(&buf, "%", true)
    @assert(buf.toString() == "true")

    buf.clear()
    format.append(&buf, "%", "1"'rune)
    @assert(buf.toString() == "1")

    //format(&buf, "%", &buf)

    //str := buf.toString()
    //Console.write(cast(string) str)
}
