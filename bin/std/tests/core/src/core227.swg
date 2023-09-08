
using Core

//#[Swag.PrintBc]
#test
{
	let filen = #location.fileName
    let attrs = catch File.getAttributes(filen)
    @assert(@errmsg == null)
    @assert(cast(bool) (attrs & File.FileAttributes.Archive))

    let infos = catch File.getInfo(filen)
    @assert(@errmsg == null)
}