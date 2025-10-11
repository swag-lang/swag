#global public
#global namespace File
using Core.Errors

// Origin from which to calculate seek offset in a file stream
enum SeekOrigin
{
    Begin       // Seek from the beginning of the file
    Current     // Seek from the current position
    End         // Seek from the end of the file
}

#[Swag.EnumFlags]
// File access permissions
enum FileAccess
{
    Read                         // Read access
    Write                        // Write access
    ReadWrite = Read | Write     // Read and write access
}

// File creation and opening behaviors
enum FileMode
{
    Append           // Opens the file if it exists and moves the seek to the end, or creates a new file
    Create           // Creates a new file, overwriting if it already exists
    CreateNew        // Creates a new file, throws if the file already exists
    Open             // Opens an existing file
    OpenOrCreate     // Opens the file if it exists, otherwise creates a new one
    Truncate         // Opens an existing file and truncates its content to zero length
}

#[Swag.EnumFlags]
// File sharing options
enum FileShare
{
    Zero        = 0                // No sharing
    Delete                         // Allows the file to be deleted by others
    Inheritable                    // File handle is inheritable
    Read                           // Allows shared read access
    Write                          // Allows shared write access
    ReadWrite   = Read | Write     // Allows shared read and write access
}

// Reads the entire content of a file into a UTF-8 string
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
            result += '\n''u8
        }

        return result
    }

    return textReader.readToEnd()
}

// Reads all lines from a file into an array of strings
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

// Executes user code for each line read from a file
#[Swag.Macro]
func readEachLines(fullname: string, encoding = TextEncoding.Utf8, stmt: #code void) throw
{
    var textReader: TextReader
    textReader.open(fullname, encoding)

    var res: String
    while textReader.readLine(&res)
    {
        #macro
        {
            var #alias0 = #ref &#up res
            #inject(#up stmt)
        }
    }
}

// Reads the entire content of a file into an array of bytes
func readAllBytes(fullname: string)->Array'u8 throw
{
    var stream = openRead(fullname)

    var result: retval
    let len     = stream.getSize()
    result.reserve(len)
    result.count = stream.read(result.buffer, len)
    return result
}

// Writes the content of a byte slice to a file
#[Swag.Discardable, Swag.Overload]
func writeAllBytes(fullname: string, buffer: const [..] u8)->u64 throw
{
    var stream = openWrite(fullname)
    return stream.write(@dataof(buffer), @countof(buffer))
}

// Writes the content of a ConcatBuffer to a file
#[Swag.Discardable, Swag.Overload]
func writeAllBytes(fullname: string, buffer: ConcatBuffer)->u64 throw
{
    var stream = openWrite(fullname)

    var total = 0'u64
    foreach p in buffer do
        total += stream.write(p.datas, p.count)

    return total
}

// Writes an array of strings to a file, one string per line
#[Swag.Discardable]
func writeAllLines(fullname: string, lines: Array'string)->u64 throw
{
    var stream = openWrite(fullname)

    var total = 0'u64
    foreach v in lines
    {
        total += stream.write(@dataof(v), @countof(v))
        total += stream.write(@dataof("\n"), 1)
    }

    return total
}

// Creates a duplicate of an existing file
func duplicate(newName, oldName: string) throw
{
    var res = readAllBytes(oldName)
    writeAllBytes(newName, res.toSlice())
}

// Retrieves information about a specific file
func getFileInfo(fullName: string)->FileInfo throw
{
    var options: Directory.EnumerationOptions
    options.matchFileName   = Path.getFileName(fullName)
    options.wantDirectories = false
    options.recurse         = false
    let file = Directory.enumerate(Path.getDirectoryName(fullName), options)
    if file.count == 0 do
        throw FileNotFoundError{fileName: Errors.mkString(fullName)}
    return file[0]
}
