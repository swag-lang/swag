using core

#test
{
    data := imagePath("rgb.bmp")
    bytes := catch file.readAllBytes(data)
    @assert(!@err)
}