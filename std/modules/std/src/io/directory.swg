public namespace std.directory;

struct EnumerationOptions
{
    searchPattern:      string
    recurse:            bool
    wantFiles:          bool = true
    wantDirectories:    bool = true
}