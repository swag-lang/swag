#import "core"
using core

//#[swag.printbc]
#test
{
	filen := #location.fileName
    (attrs, err) := file.getAttributes(filen)
    @assert(!err)
    @assert(cast(bool) (attrs & file.FileAttributes.Archive))

    (infos, err1) := file.getInfo(filen)
    @assert(!err1)
}