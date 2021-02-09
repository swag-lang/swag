using core

#test
{
    data := imagePath("image1.bmp")
    bytes := file.readAllBytes(data)
    @assert(!@err)
}