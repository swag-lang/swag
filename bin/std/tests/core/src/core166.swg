using Core

//#[Swag.PrintBc]
#test
{
    var loc  = #location
    var info = catch File.getInfo(loc.fileName)
}
