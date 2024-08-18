#global public
#global namespace File
using Core.Errors

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
    Zero        = 0
    Delete
    Inheritable
    Read
    Write
    ReadWrite   = Read | Write
}

// Read a file, and store the result in an utf8 string.
func readAllText(fullname: string, encoding = TextEncoding.Utf8, normalizeEol = false)->String throw
{
    var textReader: TextReader
    textReader.open(fullname, encoding)

    // Normalized end of lines.
    if normalizeEol
    {
        var result: retval
        var res:    String
        while textReader.readLine(&res)
        {
            result += res
            result += `\n`'u8
        }

        return result
    }

    return textReader.readToEnd()
}

// Read all the lines of a file
func readAllLines(fullname: string, encoding = TextEncoding.Utf8)->Array'String throw
{
    var textReader: TextReader
    textReader.open(fullname, encoding)

    var result: retval
    var res:    String
    while textReader.readLine(&res) do
        result.add(moveref &res)
    return result
}

// Call user code for each line
#[Swag.Macro]
func readEachLines(fullname: string, encoding = TextEncoding.Utf8, stmt: code) throw
{
    var textReader: TextReader
    textReader.open(fullname, encoding)

    var res: String
    while textReader.readLine(&res)
    {
        #macro
        {
            var #alias0 = #ref &#up res
            #mixin #up stmt
        }
    }
}

// Read a file, and store the result in an array of bytes
func readAllBytes(fullname: string)->Array'u8 throw
{
    var stream = openRead(fullname)

    var result: retval
    let len     = stream.getSize()
    result.reserve(len)
    result.count = stream.read(result.buffer, len)
    return result
}

// Write a file with the content of a slice of bytes
#[Swag.Discardable, Swag.Overload]
func writeAllBytes(fullname: string, buffer: const [..] u8)->u64 throw
{
    var stream = openWrite(fullname)
    return stream.write(@dataof(buffer), @countof(buffer))
}

// Write a file with the content of a 'ConcatBuffer'
#[Swag.Discardable, Swag.Overload]
func writeAllBytes(fullname: string, buffer: ConcatBuffer)->u64 throw
{
    var stream = openWrite(fullname)

    var total = 0'u64
    visit p in buffer do
        total += stream.write(p.datas, p.count)

    return total
}

// Write a file with the content of an array of string, one per line
#[Swag.Discardable]
func writeAllLines(fullname: string, lines: Array'string)->u64 throw
{
    var stream = openWrite(fullname)

    var total = 0'u64
    visit v in lines
    {
        total += stream.write(@dataof(v), @countof(v))
        total += stream.write(@dataof("\n"), 1)
    }

    return total
}

// Duplicate a file
func duplicate(newName, oldName: string) throw
{
    var res = readAllBytes(oldName)
    writeAllBytes(newName, res.toSlice())
}

// Get informations about a file
func getFileInfo(fullName: string)->FileInfo throw
{
    var file = Directory.enumerateFiles(Path.getDirectoryName(fullName), Path.getFileName(fullName))
    if file.count == 0 do
        throw FileNotFoundError{fileName: Errors.mkString(fullName)}
    return file[0]
}
