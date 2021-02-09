#import "image"
using core

func dataPath()->String
{
    dataPath := path.getDirectoryName(#location.fileName)
    dataPath = path.getDirectoryName(dataPath)
    return path.combine(dataPath, "datas")
}

//#[swag.printbc]
#test
{
    datas := path.combine(dataPath(), "image1.bmp")
    (bytes, err) := file.readAllBytes(datas)
    @assert(!err)
}