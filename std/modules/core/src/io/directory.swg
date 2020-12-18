public namespace directory;

struct EnumerationOptions
{
    matchPattern =           "*"
    skipAttributes =         file.FileAttributes.None
    recurse =                false
    wantFiles =              true
    wantDirectories =        true
    wantSpecialDirectories = false
}

// Get all the files of the given directory
func enumerateFiles(dirname: string, matchPattern = "*", recurse = false)->Array'(file.FileInfo)
{
    var options: EnumerationOptions
    options.matchPattern = matchPattern
    options.wantDirectories = false
    options.recurse = recurse
    return enumerate(dirname, options)
}

// Get all the directories in the given directory
func enumerateDirectories(dirname: string, matchPattern = "*", recurse = false)->Array'(file.FileInfo)
{
    var options: EnumerationOptions
    options.matchPattern = matchPattern
    options.wantFiles = false
    options.recurse = recurse
    return enumerate(dirname, options)
}
