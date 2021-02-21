using core

#test
{
    data := imagePath("bmp_24.bmp")
    bytes := catch file.readAllBytes(data)
    @assert(!@err)
}