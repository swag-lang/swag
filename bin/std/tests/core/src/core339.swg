using Core

struct S
{
    fullname: String
}

#test
{
    var allFiles: Array'S
    var allLines: Array'string
    foreach fff in allFiles:
        allLines.add(cast(string) fff.fullname)
}
