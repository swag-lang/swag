using core

#test
{
    data := imagePath("image1.bmp")
    bytes := file.readAllBytes(data) catch {}
    @assert(!@err)
}