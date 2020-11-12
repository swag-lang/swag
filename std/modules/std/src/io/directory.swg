public namespace std.directory;

struct EnumerateOptions
{
    searchPattern:      string
    recurse:            bool
    wantFiles:          bool = true
    wantDirectories:    bool = true
}