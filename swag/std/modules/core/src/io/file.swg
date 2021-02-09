#global public
#global namespace file

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
func readAllText(fullname: string, encoding = TextEncoding.Utf8)->String
{
    var result:     retval
    var textReader: TextReader

    textReader.open(fullname, encoding)
    if @err return result
    defer textReader.close()

    result = textReader.readToEnd()
    return result
}

// Read all the lines of a file
func readAllLines(fullname: string, encoding = TextEncoding.Utf8)->Array'String
{
    var result:     retval
    var textReader: TextReader

    textReader.open(fullname, encoding)
    if @err return result
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
func readAllBytes(fullname: string)->Array'u8
{
    var result: retval

    stream := openRead(fullname)
    if @err return result

    debug.assert(stream.isOpen())
    defer stream.close()

    len := getSize(stream)
    if @err return result

    result.reserve(len)
    result.count = read(stream, result.buffer, len)
    return result
}