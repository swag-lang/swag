#global public
#global namespace Directory

struct EnumerationOptions
{
    matchPattern =           "*"
    skipAttributes =         File.FileAttributes.Zero
    recurse =                false
    wantFiles =              true
    wantDirectories =        true
    wantSpecialDirectories = false

    filterLambda: func(File.FileInfo)->bool
}

// Get all the files of the given directory
func enumerateFiles(dirName: string, matchPattern = "*", recurse = false)->Array'(File.FileInfo) throw
{
    var options: EnumerationOptions
    options.matchPattern = matchPattern
    options.wantDirectories = false
    options.recurse = recurse
    return try enumerate(dirName, options)
}

// Get all the directories in the given directory
func enumerateDirectories(dirName: string, matchPattern = "*", recurse = false)->Array'(File.FileInfo) throw
{
    var options: EnumerationOptions
    options.matchPattern = matchPattern
    options.wantFiles = false
    options.recurse = recurse
    return try enumerate(dirName, options)
}
