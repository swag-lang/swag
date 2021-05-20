using core

#test
{
    data := imagePath("rgb.bmp")
    bytes := catch File.readAllBytes(data)
    @assert(!@err)
}