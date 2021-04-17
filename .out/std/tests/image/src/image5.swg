using image

//#[swag.printbc]
#test
{
    img := try image.load(imagePath("jpg_24.jpg"))
    @assert(img.width == 660)
    @assert(img.height == 441)
}