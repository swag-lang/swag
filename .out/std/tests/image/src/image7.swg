using image

//#[swag.printbc]
#test
{
    img := try image.load(imagePath("rgba.png"))
    //@assert(img.width == 660)
    //@assert(img.height == 441)
}