#global public
#global namespace Directory

// Options for customizing file and directory enumeration
struct EnumerationOptions
{
    matchExtension:            string                         // Optional extension to match (e.g. ".txt")
    matchFileName:             string                         // Optional file name pattern to match
    skipAttributes             = File.FileAttributes.Zero     // Attributes to skip during enumeration
    recurse                    = false                        // Whether to recurse into subdirectories
    wantFiles                  = true                         // Whether to include files in the results
    wantDirectories            = true                         // Whether to include directories in the results
    wantSpecialDirectories     = false                        // Whether to include "." and ".." in the results

    filterLambda:              func(File.FileInfo)->bool      // Optional lambda to filter results
}

// Gets all files in the specified directory
func enumerateFiles(dirName: string, matchExtension: string = null, recurse = false)->Array'(File.FileInfo) throw
{
    var options: EnumerationOptions
    options.matchExtension  = matchExtension
    options.wantDirectories = false
    options.recurse         = recurse
    return try enumerate(dirName, options)
}

// Gets all directories in the specified directory
func enumerateDirectories(dirName: string, matchExtension: string = null, recurse = false)->Array'(File.FileInfo) throw
{
    var options: EnumerationOptions
    options.matchExtension = matchExtension
    options.wantFiles      = false
    options.recurse        = recurse
    return try enumerate(dirName, options)
}
