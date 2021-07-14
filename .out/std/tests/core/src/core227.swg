
using Core

//#[Swag.PrintBc]
#test
{
	filen := #location.fileName
    attrs := catch File.getAttributes(filen)
    @assert(@err == null)
    @assert(cast(bool) (attrs & File.FileAttributes.Archive))

    infos := catch File.getInfo(filen)
    @assert(@err == null)
}