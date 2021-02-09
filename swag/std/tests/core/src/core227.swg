#import "core"
using core

//#[swag.printbc]
#test
{
	filen := #location.fileName
    attrs := file.getAttributes(filen)
    @assert(!@err)
    @assert(cast(bool) (attrs & file.FileAttributes.Archive))

    infos := file.getInfo(filen)
    @assert(!@err)
}