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
    enableCache:        bool = true
    inPause:            bool
    mustLoop:           bool = true
    playFreq:           f32 = 0.05
    sigFrameChanged:    SigArray'closure(*Self)

    numFrames:      s32
    imgList:        ImageList
    source:         MovieSource = None
    frameIndex:     s32
    currentTime:    f32
    gif:            Gif.Decoder
    bytes:          Array'u8
    frame:          Image
    cacheFrames:    Array'Image
    dirtyFrame:     bool
}

private impl Movie
{
    mtd nextFrame()
    {
        frameIndex += 1

        #[Swag.Complete]
        switch source
        {
        case None:
            break
        case SpriteSheet:
            if frameIndex >= imgList.totalFrames
            {
                if mustLoop
                    frameIndex = 0
                else
                    frameIndex = imgList.totalFrames - 1
            }

        case Gif:
            if numFrames and enableCache
            {
                if frameIndex >= numFrames
                {
                    if mustLoop
                        frameIndex = 0
                    else
                        frameIndex = imgList.totalFrames - 1
                }

                frame = dref cacheFrames[frameIndex]
            }
            elif assume gif.nextFrame(&frame) == false
            {
                numFrames = frameIndex
                if mustLoop
                {
                    gif.rewind()
                    frameIndex = 0
                    if enableCache
                        frame = dref cacheFrames[0]
                    else
                        discard assume gif.nextFrame(&frame)
                }
            }
            elif enableCache
            {
                cacheFrames.add(frame)
            }
        }

        sigFrameChanged.call(self)
    }
}

impl Movie
{
    mtd clear()
    {
        cacheFrames.clear()
        bytes.clear()
        source = .None
        frame.release()
        numFrames = 0
        frameIndex = 0
        currentTime = 0
    }

    // Associate an image list
    mtd set(imageList: ImageList)
    {
        clear()
        source = .SpriteSheet
        imgList   = imageList
        numFrames = imgList.totalFrames
    }

    // Associate a filename
    mtd set(fileName: string) throw
    {
        clear()
        if gif.IImageDecoder.canDecode(fileName)
        {
            source = .Gif
            bytes = try File.readAllBytes(fileName)
            try gif.init(bytes.toSlice(), true)
            gif.createFrame(&frame)
            discard try gif.nextFrame(&frame)
            if enableCache
                cacheFrames.add(frame)
        }
        else
        {
            throw "unsupported format"
        }
    }

    // Returns true if the movie has valid content
    mtd isValid()->bool
    {
        #[Swag.Complete]
        switch source
        {
        case None:
            return false
        case SpriteSheet:
            return imgList.texture.valid
        case Gif:
            return frame.pixels != null
        }

        return false
    }

    // Set the current video frame
    mtd setFrameIndex(idx: s32)
    {
        if idx == frameIndex
            return
        frameIndex = Math.min(idx, numFrames)

        #[Swag.Complete]
        switch source
        {
        case None:
            break
        case Gif:
            frameIndex = Math.min(idx, numFrames)
            if frameIndex < cacheFrames.count
            {
                frame = dref cacheFrames[frameIndex]
                dirtyFrame = true
            }

        case SpriteSheet:
            break
        }

        sigFrameChanged.call(self)
    }

    // Set/Reset pause state
    mtd pause(state: bool)
    {
        inPause = state
    }

    // Update timing and change frames
    mtd update(dt: f32)->bool
    {
        wasDirty := dirtyFrame
        dirtyFrame = false

        if inPause
            return wasDirty

        hasChanged := false
        currentTime += dt
        while currentTime > playFreq
        {
            hasChanged = true
            currentTime -= playFreq
            nextFrame()
        }

        return hasChanged or wasDirty
    }
}
