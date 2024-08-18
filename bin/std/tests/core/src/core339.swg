using Core

struct S
{
    fullname: String
}

#test
{
    var allFiles: Array'S
    var allLines: Array'string
    visit fff in allFiles do
        allLines.add(cast(string) fff.fullname)
}
