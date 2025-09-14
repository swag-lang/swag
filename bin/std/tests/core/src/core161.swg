using Core

//#[Swag.PrintBc]
#test
{
    let loc = #curlocation
    var res = catch File.readAllBytes(loc.fileName)
    //Console.print("%", res.item0)
}
