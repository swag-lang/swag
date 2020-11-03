public namespace std.file;

enum SeekOrigin
{
    Begin
    Current
    End
}

// Read a file, and store the result in an utf8 string
//#[swag.printbc]
func readAllText(fullname: string, encoding = TextEncoding.Utf8) -> {String, bool}
{
    var result: String
    var textReader: TextReader
    if !textReader.open(fullname, encoding)
        return @{result, false}
    return @{textReader.readToEnd(), true}
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