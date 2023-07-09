
using Core

//#[Swag.PrintBc]
#test
{
	var filen = #location.fileName
    var attrs = catch File.getAttributes(filen)
    @assert(@errmsg == null)
    @assert(cast(bool) (attrs & File.FileAttributes.Archive))

    var infos = catch File.getInfo(filen)
    @assert(@errmsg == null)
}