using core

#test
{
    data := imagePath("image1.bmp")
    (bytes, err) := file.readAllBytes(data)
    @assert(!err)
}