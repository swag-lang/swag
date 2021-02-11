using core

#test
{
    data := imagePath("image1.bmp")
    bytes := catch file.readAllBytes(data)
    @assert(!@err)
}