using Pixel

//#[Swag.printbc]
#test
{
    img := try Image.load(imagePath("h2v1.jpg"))
    @assert(img.width == 660)
    @assert(img.height == 441)

    img1 := try Image.load(imagePath("progh1v1.jpg"))
    @assert(img1.width == 660)
    @assert(img1.height == 441)
}