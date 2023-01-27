#global public
#global namespace File

enum SeekOrigin
{
    Begin
    Current
    End
}

#[Swag.EnumFlags]
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

#[Swag.EnumFlags]
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
    var textReader: TextReader
    textReader.open(fullname, encoding)
    defer textReader.close()

    var result: retval
    var res: String
    while textReader.readLine(&res)
        result.emplace(&res)
    return result
}

// Call user code for each line
#[Swag.Macro]
func readEachLines(fullname: string, encoding = TextEncoding.Utf8, stmt: code) throw
{
    var textReader: TextReader
    textReader.open(fullname, encoding)
    defer textReader.close()

    var res: String
    while textReader.readLine(&res)
    {
        #macro
        {
            var @alias0 = ref &`res
            #mixin `stmt
        }
    }
}

// Read a file, and store the result in an array of bytes
func readAllBytes(fullname: string)->Array'u8 throw
{
    stream := try openRead(fullname)
    Debug.assert(stream.isOpen())
    defer stream.close()

    var result: retval
    len := try stream.getSize()
    result.reserve(len)
    result.count = stream.read(result.buffer, len)

    return result
}

// Write a file with the content of a slice of bytes
#[Swag.Discardable]
func writeAllBytes(fullname: string, buffer: const [..] u8)->uint throw
{
    stream := try openWrite(fullname)
    Debug.assert(stream.isOpen())
    defer stream.close()

    return try stream.write(@dataof(buffer), @countof(buffer))
}

// Write a file with the content of a 'ConcatBuffer'
#[Swag.Discardable]
func writeAllBytes(fullname: string, buffer: ConcatBuffer)->uint throw
{
    stream := try openWrite(fullname)
    Debug.assert(stream.isOpen())
    defer stream.close()

    total := 0'uint
    visit p: buffer
        total += try stream.write(p.datas, p.count)

    return total
}

// Write a file with the content of an array of string, one per line
#[Swag.Discardable]
func writeAllLines(fullname: string, lines: Array'string)->uint throw
{
    stream := try openWrite(fullname)
    Debug.assert(stream.isOpen())
    defer stream.close()

    total := 0'uint
    visit v: lines
    {
        total += try stream.write(@dataof(v), @countof(v))
        total += try stream.write(@dataof("\n"), 1)
    }

    return total
}

// Duplicate a file
func duplicate(newName, oldName: string) throw
{
    res := readAllBytes(oldName)
    writeAllBytes(newName, res.toSlice())
}

// Get informations about a file
func getFileInfo(fullName: string)->FileInfo throw
{
    file := Directory.enumerateFiles(Path.getDirectoryName(fullName), Path.getFileName(fullName))
    if file.count == 0
        throw "cannot find file"
    return file[0]
}