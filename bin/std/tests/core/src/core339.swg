using Core

struct S
{
    fullname: String
}

#test
{
    let allFiles: Array'S
    var allLines: Array'string
    foreach fff in allFiles:
        allLines.add(cast(string) fff.fullname)
}
