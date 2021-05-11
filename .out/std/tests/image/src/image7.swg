using image

//#[swag.printbc]
#test
{
    img := try image.load(imagePath("rgb.png"))
    @assert(img.width == 660)
    @assert(img.height == 441)
}