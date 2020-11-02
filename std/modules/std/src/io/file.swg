public namespace std.file;

enum SeekOrigin
{
    Begin
    Current
    End
}

#[swag.printbc]
func readAllText(fullname: string) -> {String, bool}
{
    var result: String

    stream := openRead(fullname)
    if !stream.isOpen() return @{result, false}

    len := getSize(stream)
    debug.assert(len <= swag.MaxU32)
    len32 := cast(u32) len

    result.reserve(len32)
    result.length = len32

    read(stream, result.buffer, len32)

    close(&stream)
    return @{result, true}
}