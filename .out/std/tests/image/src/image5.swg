using image

//#[swag.printbc]
#test
{
    img := try image.load(imagePath("yh2v1.jpg"))
    @assert(img.width == 660)
    @assert(img.height == 441)
}