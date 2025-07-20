using Core, Pixel

#test
{
    var img1 = try Image.load(imagePath("haar1.png"))
    let res1 = ImageStatistics.quickHaarCompareTo(img1, img1)
    @assert(res1 == 1.0)

    var img2 = try Image.load(imagePath("haar2.png"))
    let res2 = ImageStatistics.quickHaarCompareTo(img1, img2)
    @assert(res2 > 0.6)

    var img3 = try Image.load(imagePath("rgb.png"))
    let res3 = ImageStatistics.quickHaarCompareTo(img1, img3)
    @assert(res3 < 0.6)

    @print(res1, " ", res2, " ", res3)
}

#test
{
    var img1 = try Image.load(imagePath("haar1.png"))
    let res1 = ImageStatistics.haarCompareTo(img1, img1)
    @assert(res1 == 1.0)

    var img2 = try Image.load(imagePath("haar2.png"))
    let res2 = ImageStatistics.haarCompareTo(img1, img2)
    @assert(res2 > 0.5)

    var img3 = try Image.load(imagePath("rgb.png"))
    let res3 = ImageStatistics.haarCompareTo(img1, img3)
    @assert(res3 < 0.5)

    @print(res1, " ", res2, " ", res3)
}
