public namespace std.file;

enum SeekOrigin
{
    Begin
    Current
    End
}

//#[swag.printbc]
func readAllText(fullname: string) -> {String, bool}
{
    var result: String

    stream := openRead(fullname)
    if !stream.isOpen() return @{result, false}

    len := getSize(stream)
    debug.assert(len <= swag.MaxU32)
    len32 := cast(u32) len

    result.reserve(len32)
    result.length = read(stream, result.buffer, len32)

    return @{result, true}
}

// Read a file, and store the result in an array of bytes
func readAllBytes(fullname: string) -> {Array'u8, bool}
{
    var result: Array'u8

    stream := openRead(fullname)
    if !stream.isOpen() return @{result, false}

    len := getSize(stream)
    debug.assert(len <= swag.MaxU32)
    len32 := cast(u32) len

    result.reserve(len32)
    result.count = read(stream, result.buffer, len32)

    return @{result, true}
}