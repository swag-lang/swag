#global public
using Core, Pixel

enum MovieSource
{
    None
    SpriteSheet
    Gif
}

struct Movie
{
    mustLoop:       bool = true
    playFreq:       f32 = 0.05

    numFrames:      s32
    imgList:        ImageList
    source:         MovieSource = None
    frameIndex:     s32
    currentTime:    f32
    gif:            Gif.Decoder
    bytes:          Array'u8
    frame:          Image
}

private impl Movie
{
    mtd nextFrame()
    {
        frameIndex += 1
        switch source
        {
        case SpriteSheet:
            if frameIndex >= imgList.totalFrames
                frameIndex = 0
        case Gif:
            if assume gif.nextFrame(&frame) == false
            {
                numFrames = frameIndex
                if mustLoop
                {
                    gif.rewind()
                    frameIndex = 0
                    discard assume gif.nextFrame(&frame)
                }
            }
        }
    }
}

impl Movie
{
    mtd set(imageList: ImageList)
    {
        source = .SpriteSheet
        frame.release()
        imgList   = imageList
        numFrames = imgList.totalFrames
        rewind()
    }

    mtd set(fileName: string) throw
    {
        frame.release()
        numFrames = 0

        if gif.IImageDecoder.canDecode(fileName)
        {
            source = .Gif
            bytes.clear()
            bytes = try File.readAllBytes(fileName)
            try gif.init(bytes.toSlice(), true)
            gif.createFrame(&frame)
            discard try gif.nextFrame(&frame)
            frameIndex = 0
        }
    }

    mtd isValid()->bool
    {
        switch source
        {
        case SpriteSheet:
            return imgList.texture.valid
        case Gif:
            return frame.pixels != null
        }

        return false
    }

    mtd load(file: string) throw
    {
        rewind()
    }

    mtd rewind()
    {
        frameIndex = -1
        currentTime = 0
        nextFrame()
    }

    mtd update(dt: f32)->bool
    {
        hasChanged := false
        currentTime += dt
        while currentTime > playFreq
        {
            hasChanged = true
            currentTime -= playFreq
            nextFrame()
        }

        return hasChanged
    }
}
