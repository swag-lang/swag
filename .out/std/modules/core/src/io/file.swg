#global public
#global namespace File

enum SeekOrigin
{
    Begin
    Current
    End
}

#[Swag.enumflags]
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

#[Swag.enumflags]
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
func readAllText(fullname: string, encoding = TextEncoding.Utf8)->String throw
{
    var result:     retval
    var textReader: TextReader

    try textReader.open(fullname, encoding)
    defer textReader.close()

    result = textReader.readToEnd()
    return result
}

// Read all the lines of a file
func readAllLines(fullname: string, encoding = TextEncoding.Utf8)->Array'String throw
{
    var result:     retval
    var textReader: TextReader

    try textReader.open(fullname, encoding)
    defer textReader.close()

    res := textReader.readLine()
    while !res.isNull()
    {
        result.add(res)
        res = textReader.readLine()
    }

    return result
}

// Read a file, and store the result in an array of bytes
func readAllBytes(fullname: string)->Array'u8 throw
{
    var result: retval

    stream := try openRead(fullname)
    Debug.assert(stream.isOpen())
    defer stream.close()

    len := try getSize(stream)
    result.reserve(len)
    result.count = read(stream, result.buffer, len)

    return result
}

// Write a file with the content of a slice of bytes
#[Swag.discardable]
func writeAllBytes(fullname: string, buffer: const [..] u8)->uint throw
{
    var result: retval

    stream := try openWrite(fullname)
    Debug.assert(stream.isOpen())
    defer stream.close()

    return try write(stream, @dataof(buffer), @countof(buffer))
}