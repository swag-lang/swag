using Core

#test
{
    var loc = #curlocation
    var dir = Path.getDirectoryName(loc.fileName)
    var res = catch Directory.enumerateDirectories(#"F:\swag\std\output"#, recurse: true)
    //@assert(res.count == 1)
    //foreach &c in res
    //    Console.print("FF: % %\n", c.fullname, c.size)
}
