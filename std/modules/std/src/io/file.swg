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

#[swag.enumflags]
enum FileAttribute
{
    None
    ReadOnly
    Hidden
    System
    Directory
    Archive
    Device
    Normal
    Temporary
    SparseFile
    ReparsePoint
    Compressed
    Offline
    NotContentIndexed
    Encrypted
}

// Read a file, and store the result in an utf8 string
func readAllText(fullname: string, encoding = TextEncoding.Utf8) -> {text: String, success: bool}
{
    var textReader: TextReader
    if !textReader.open(fullname, encoding)
        return @{String{}, false}
    return @{textReader.readToEnd(), true}
}

// Read all the lines of a file
func readAllLines(fullname: string, encoding = TextEncoding.Utf8) -> {lines: Array'String, success: bool}
{
    var result: Array'String

    var textReader: TextReader
    if !textReader.open(fullname, encoding)
        return @{result, false}

    res := textReader.readLine()
    while !res.isNull()
    {
        result.add(res)
        res = textReader.readLine()
    }

    return @{result, true}
}

// Read a file, and store the result in an array of bytes
func readAllBytes(fullname: string) -> {bytes: Array'u8, success: bool}
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