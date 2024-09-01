using Core

//#[Swag.PrintBc]
#test
{
    var loc  = #curlocation
    var info = catch File.getInfo(loc.fileName)
    /*Console.print("size: %\n", info.size)
    Console.print("attr: %\n", info.attributes)
    Console.print("crt:  %\n", info.creationTime)
    Console.print("lwt:  %\n", info.lastWriteTime)*/
}
