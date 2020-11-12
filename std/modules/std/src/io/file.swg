public namespace std.file;

enum SeekOrigin
{
    Begin
    Current
    End
}

#[swag.enumflags]
enum FileAccess
{
    Read
    Write
    ReadWrite = Read | Write
}

enum FileMode
{
    Append
    Create
    CreateNew
    Open
    OpenOrCreate
    Truncate
}

#[swag.enumflags]
enum FileShare
{
    None
    Delete
    Inheritable
    Read
    Write
    ReadWrite = Read | Write
}

// Read a file, and store the result in an utf8 string
func readAllText(fullname: string, encoding = TextEncoding.Utf8) -> {text: String, success: bool}
{
    var result:     retval
    var textReader: TextReader
    if !textReader.open(fullname, encoding)
        return result

    result.text = textReader.readToEnd()
    result.success = true
    return result
}

// Read all the lines of a file
func readAllLines(fullname: string, encoding = TextEncoding.Utf8) -> {lines: Array'String, success: bool}
{
    var result:     retval
    var textReader: TextReader
    if !textReader.open(fullname, encoding)
        return result

    res := textReader.readLine()
    while !res.isNull()
    {
        result.lines.add(res)
        res = textReader.readLine()
    }

    result.success = true
    return result
}

// Read a file, and store the result in an array of bytes
func readAllBytes(fullname: string) -> {bytes: Array'u8, success: bool}
{
    var result: retval

    stream := openRead(fullname)
    if !stream.isOpen()
        return result

    len := getSize(stream)
    debug.assert(len <= swag.MaxU32)
    len32 := cast(u32) len

    result.bytes.reserve(len32)
    result.bytes.count = read(stream, result.bytes.buffer, len32)

    result.success = true
    return result
}