#global namespace ImageStatistics
using Core, Core.Math

func haarTransform2D(data: [*] f32, size: u32, output: [*] f32)
{
    Memory.copy(output, data, size * size * #sizeof(f32))

    // Decompose by levels
    var currentSize = size
    while currentSize > 1
    {
        for row in currentSize do
            haarTransform1D(output, row * size, currentSize)
        for col in currentSize do
            haarTransform1DVertical(output, col, currentSize, size)
        currentSize /= 2
    }
}

// 1D Haar transform on a row
func haarTransform1D(data: [*] f32, offset: s32, length: s32)
{
    if length <= 1 do
        return

    var temp: Array'f32
    temp.resize(length)

    let half = length / 2

    for i in half do
        temp[i] = (data[offset + i * 2] + data[offset + i * 2 + 1]) * 0.70710678118 // sqrt(2)/2 normalization
    for i in half do
        temp[half + i] = (data[offset + i * 2] - data[offset + i * 2 + 1]) * 0.70710678118

    for i in length do
        data[offset + i] = temp[i]
}

// 1D Haar transform on a column
func haarTransform1DVertical(data: [*] f32, col: s32, length: s32, stride: s32)
{
    if length <= 1 do
        return

    var temp: Array'f32
    temp.resize(length)

    let half = length / 2

    for i in half do
        temp[i] = (data[col + i * 2 * stride] + data[col + (i * 2 + 1) * stride]) * 0.70710678118
    for i in half do
        temp[half + i] = (data[col + i * 2 * stride] - data[col + (i * 2 + 1) * stride]) * 0.70710678118

    for i in length do
        data[col + i * stride] = temp[i]
}

func resampleToGrayscale(image: Image, targetWidth, targetHeight: s32, output: [*] f32)
{
    Debug.assert(targetWidth > 0 and targetHeight > 0, "Target dimensions must be positive")

    for y in targetHeight
    {
        for x in targetWidth
        {
            // Bilinear sampling
            let srcX = cast(f32) (x * image.width) / cast(f32) targetWidth
            let srcY = cast(f32) (y * image.height) / cast(f32) targetHeight

            let x0 = cast(s32) srcX
            let y0 = cast(s32) srcY
            let x1 = Math.min(x0 + 1, image.width - 1)
            let y1 = Math.min(y0 + 1, image.height - 1)

            let fx = srcX - cast(f32) x0
            let fy = srcY - cast(f32) y0

            // Bilinear interpolation
            let c00 = image.getPixelColor(x0, y0)
            let c10 = image.getPixelColor(x1, y0)
            let c01 = image.getPixelColor(x0, y1)
            let c11 = image.getPixelColor(x1, y1)

            let gray00 = c00.getGrayScalef()
            let gray10 = c10.getGrayScalef()
            let gray01 = c01.getGrayScalef()
            let gray11 = c11.getGrayScalef()

            let interpolated = gray00 * (1.0 - fx) * (1.0 - fy) +
                               gray10 * fx * (1.0 - fy) +
                               gray01 * (1.0 - fx) * fy +
                               gray11 * fx * fy

            output[y * targetWidth + x] = interpolated
        }
    }
}

// Structure to store Haar coefficients
public struct HaarFeatures
{
    coefficients:     Array'f32     // Multi-scale Haar coefficients
    size:             s32           // Representation size (power of 2)
    levels:           s32           // Number of decomposition levels
}

// Extract Haar coefficients from the image
public func extractHaarFeatures(image: Image, windowSize: u32 = 128)->HaarFeatures
{
    Debug.assert(Math.isPowerOf2(windowSize))
    Debug.assert(windowSize >= 32)

    var features: retval

    // Determine optimal size
    let targetSize = Math.min(Math.roundUpToPowerOf2(cast(u32) Math.min(image.width, image.height)), windowSize)
    features.size   = cast(s32) targetSize
    features.levels = cast(s32) Math.log2(cast(f32) targetSize) // Number of decomposition levels

    // Total coefficients = size * size (all levels included)
    features.coefficients.resize(targetSize * targetSize)

    // Convert image to grayscale and resize
    var grayImage: Array'f32
    grayImage.resize(targetSize * targetSize)
    image.resampleToGrayscale(cast(s32) targetSize, cast(s32) targetSize, grayImage.buffer)

    // 2D Haar transform
    haarTransform2D(grayImage.buffer, targetSize, features.coefficients.buffer)
    return features
}

// Compare Haar coefficients with multiscale weights
public func compareHaarCoefficients(haar1, haar2: HaarFeatures)->f32
{
    Debug.assert(haar1.size == haar2.size, "Haar features must have same size")

    var totalSimilarity: f64 = 0.0
    var totalWeight:     f64 = 0.0
    let size             = haar1.size

    for level in haar1.levels
    {
        let weight = Math.pow(2.0, cast(f64) (haar1.levels - level))

        var levelSimilarity: f64 = 0.0
        var levelNorm1:      f64 = 0.0
        var levelNorm2:      f64 = 0.0

        if level == 0
        {
            // DC component
            let c1 = cast(f64) haar1.coefficients[0]
            let c2 = cast(f64) haar2.coefficients[0]
            levelSimilarity = c1 * c2
            levelNorm1      = c1 * c1
            levelNorm2      = c2 * c2
        }
        else
        {
            let levelSize = size >> level

            // LH (horizontal details): top-right
            for y in 0 until levelSize do
                for x in levelSize until levelSize * 2
                {
                    let idx = y * size + x
                    let c1  = cast(f64) haar1.coefficients[idx]
                    let c2  = cast(f64) haar2.coefficients[idx]
                    levelSimilarity += c1 * c2
                    levelNorm1 += c1 * c1
                    levelNorm2 += c2 * c2
                }

            // HL (vertical details): bottom-left
            for y in levelSize until levelSize * 2 do
                for x in 0 until levelSize
                {
                    let idx = y * size + x
                    let c1  = cast(f64) haar1.coefficients[idx]
                    let c2  = cast(f64) haar2.coefficients[idx]
                    levelSimilarity += c1 * c2
                    levelNorm1 += c1 * c1
                    levelNorm2 += c2 * c2
                }

            // HH (diagonal details): bottom-right
            for y in levelSize until levelSize * 2 do
                for x in levelSize until levelSize * 2
                {
                    let idx = y * size + x
                    let c1  = cast(f64) haar1.coefficients[idx]
                    let c2  = cast(f64) haar2.coefficients[idx]
                    levelSimilarity += c1 * c2
                    levelNorm1 += c1 * c1
                    levelNorm2 += c2 * c2
                }
        }

        let denominator = Math.sqrt(levelNorm1 * levelNorm2)
        if denominator > 1e-10 do
            levelSimilarity = Math.abs(levelSimilarity / denominator)
        else do
            levelSimilarity = (levelNorm1 == 0.0 and levelNorm2 == 0.0) ? 1.0 : 0.0

        totalSimilarity += levelSimilarity * weight
        totalWeight += weight
    }

    if totalWeight > 0.0 do
        return cast(f32) (totalSimilarity / totalWeight)

    return 0.0'f32 // Considered more neutral than 1.0
}

// Compare to another image with Haar coefficients
public func haarCompareTo(one, two: Image, windowSize: u32 = 128)->f32
{
    Debug.assert(Math.isPowerOf2(windowSize))
    Debug.assert(windowSize >= 32)

    let haar1 = one.extractHaarFeatures(windowSize)
    let haar2 = two.extractHaarFeatures(windowSize)
    return compareHaarCoefficients(haar1, haar2)
}

// Extract Haar coefficients from the image (fast version)
public func quickExtractHaarFeatures(image: Image, windowSize: u32 = 32)->HaarFeatures
{
    Debug.assert(Math.isPowerOf2(windowSize))
    Debug.assert(windowSize >= 32)
    Debug.assert(image.width > 0 and image.height > 0, "Image must have positive dimensions")

    var features: retval
    features.coefficients.resize(windowSize * windowSize)
    features.size   = cast(s32) windowSize
    features.levels = cast(s32) Math.log2(cast(f32) windowSize) // Number of decomposition levels

    // Fast grayscale resizing with proper bounds checking
    var grayData: Array'f32
    grayData.resize(windowSize * windowSize)

    let stepX = Math.max(1.0, cast(f32) image.width / cast(f32) windowSize)
    let stepY = Math.max(1.0, cast(f32) image.height / cast(f32) windowSize)

    for y in windowSize
    {
        let srcY = Math.min(cast(s32) (cast(f32) y * stepY), image.height - 1)
        for x in windowSize
        {
            let srcX  = Math.min(cast(s32) (cast(f32) x * stepX), image.width - 1)
            let color = image.getPixelColor(srcX, srcY)
            let gray  = color.getGrayScalef()
            grayData.buffer[y * windowSize + x] = gray
        }
    }

    // 2D Haar transform
    haarTransform2D(grayData.buffer, windowSize, features.coefficients.buffer)
    return features
}

// Compare Haar coefficients (simplified version)
public func quickCompareHaarCoefficients(haar1, haar2: HaarFeatures)->f32
{
    Debug.assert(haar1.size == haar2.size, "Haar features must have same size")

    var norm1, norm2, correlation: f64
    let coeffCount = haar1.coefficients.count

    for i in coeffCount // Fixed: iterate over indices, not values
    {
        let c1 = cast(f64) haar1.coefficients[i]
        let c2 = cast(f64) haar2.coefficients[i]
        correlation += c1 * c2
        norm1 += c1 * c1
        norm2 += c2 * c2
    }

    let denominator = Math.sqrt(norm1 * norm2)
    if denominator > 1e-10 do
        return cast(f32) Math.abs(correlation / denominator)
    return (norm1 == 0.0 and norm2 == 0.0) ? 1.0'f32 : 0.0'f32
}

// Compare to another image with Haar coefficients, fast simplified version
public func quickHaarCompareTo(one, two: Image, windowSize: u32 = 32)->f32
{
    Debug.assert(Math.isPowerOf2(windowSize))
    Debug.assert(windowSize >= 32)

    let haar1 = one.quickExtractHaarFeatures(windowSize)
    let haar2 = two.quickExtractHaarFeatures(windowSize)
    return quickCompareHaarCoefficients(haar1, haar2)
}
