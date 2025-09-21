using Core, Pixel

#test
{
    var img = try Image.load(imagePath("1.webp"))
    @assert(img.width == 400)
    @assert(img.height == 400)
    catch img.save("c:/temp/a.png")
}
