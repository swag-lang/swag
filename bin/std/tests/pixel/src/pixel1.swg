using Core

#test
{
    var data = imagePath("rgb.bmp")
    var bytes = catch File.readAllBytes(data)
    @assert(!@err())
}