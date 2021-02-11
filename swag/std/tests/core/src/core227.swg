#import "core"
using core

//#[swag.printbc]
#test
{
	filen := #location.fileName
    attrs := catch file.getAttributes(filen)
    @assert(@err == null)
    @assert(cast(bool) (attrs & file.FileAttributes.Archive))

    infos := catch file.getInfo(filen)
    @assert(@err == null)
}