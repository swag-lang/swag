public namespace std.file;

enum SeekOrigin
{
    Begin
    Current
    End
}

func readAllText(path: string)->String
{
    var result: String

    stream := openRead(path)
    if !stream.isOpen() return result

    setPosition(stream, SeekOrigin.End, 0)
    len := cast(u32) getPosition(stream)
    setPosition(stream, SeekOrigin.Begin, 0)

    result.reserve(len)
    result.length = len

    read(stream, result.buffer, len)

    close(stream)
    return result
}