using Core

//#[Swag.PrintBc]
#test
{
    var loc = #curlocation
    var res = catch File.readAllBytes(loc.fileName)
    //Console.print("%", res.item0)
}
