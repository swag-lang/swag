public namespace file;

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
func readAllText(fullname: string, encoding = TextEncoding.Utf8) -> {text: String, err: ErrorId}
{
    var result:     retval
    var textReader: TextReader

    result.err = textReader.open(fullname, encoding)
    if result.err return result

    result.text = textReader.readToEnd()
    return result
}

// Read all the lines of a file
func readAllLines(fullname: string, encoding = TextEncoding.Utf8) -> {lines: Array'String, err: ErrorId}
{
    var result:     retval
    var textReader: TextReader

    result.err = textReader.open(fullname, encoding)
    if result.err return result

    res := textReader.readLine()
    while !res.isNull()
    {
        result.lines.add(res)
        res = textReader.readLine()
    }

    return result
}

// Read a file, and store the result in an array of bytes
func readAllBytes(fullname: string) -> {bytes: Array'u8, err: ErrorId}
{
    var result: retval

    (stream, err) := openRead(fullname)
    if err
    {
        result.err = err
        return result
    }

    debug.assert(stream.isOpen())

    (len, err1) := getSize(stream)
    if err1
    {
        result.err = err1
        return result
    }

    result.bytes.reserve(len)
    result.bytes.count = read(stream, result.bytes.buffer, len)
    return result
}