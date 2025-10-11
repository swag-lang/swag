#global public
using Core, Core.Errors, Pixel

enum MovieSource
{
    None
    SpriteSheet
    Gif
}

struct Movie
{
    enableCache:         bool = true
    mustLoop:            bool = true
    inPause:             bool
    playFreq:            f32 = 0.05
    sigFrameChanged:     SigArray'func||(*Movie)

    numFrames:           s32
    frameIndex:          s32
    frame:               Image

    source:              MovieSource = None
    imgList:             ImageList
    gif:                 Gif.Decoder
    bytes:               Array'u8
    cacheFrames:         Array'Image
    currentTime:         f32
    dirtyFrame:          bool
}

internal impl Movie
{
    mtd nextFrame()
    {
        if .frameIndex == .numFrames - 1
        {
            if .mustLoop do
                .frameIndex = 0
            else do
                return
        }
        else do
            .frameIndex += 1

        #[Swag.Complete]
        switch .source
        {
        case None:
            break
        case SpriteSheet:
            break

        case Gif:
            if .frameIndex < .cacheFrames.count
            {
                .frame = .cacheFrames[.frameIndex]
            }
            elif assume .gif.nextFrame(&.frame) == false
            {
                if .mustLoop
                {
                    .gif.rewind()
                    .frameIndex = 0
                    discard assume .gif.nextFrame(&.frame)
                }
            }
            elif .enableCache
            {
                .cacheFrames.add(.frame)
            }
        }

        .sigFrameChanged.call(me)
    }
}

impl Movie
{
    mtd clear()
    {
        .cacheFrames.clear()
        .bytes.clear()
        .source = .None
        .frame.release()
        .numFrames   = 0
        .frameIndex  = 0
        .currentTime = 0
    }

    // Associate an image list
    #[Swag.Overload]
    mtd set(imageList: ImageList)
    {
        .clear()
        .source    = .SpriteSheet
        .imgList   = imageList
        .numFrames = .imgList.totalFrames
    }

    // Associate a filename
    #[Swag.Overload]
    mtd set(fileName: string) throw
    {
        .clear()
        if .gif.IImageDecoder.canDecode(fileName)
        {
            .source = .Gif
            .bytes  = File.readAllBytes(fileName)
            .gif.init(.bytes.toSlice(), true)
            .numFrames = .gif.getNumFrames()
            .gif.getFrame(&.frame)
            discard .gif.nextFrame(&.frame)
            if .enableCache do
                .cacheFrames.add(.frame)
        }
        else
        {
            throw UnsupportedFormatError{"unsupported format"}
        }
    }

    // Returns true if the movie has valid content
    mtd isValid()->bool
    {
        #[Swag.Complete]
        switch .source
        {
        case None:
            return false
        case SpriteSheet:
            return .imgList.texture.valid
        case Gif:
            return .frame.pixels.buffer != null
        }

        return false
    }

    // Set the current video frame
    mtd setFrameIndex(idx: s32)
    {
        if idx == .frameIndex do
            return
        .frameIndex = Math.min(idx, .numFrames)

        #[Swag.Complete]
        switch .source
        {
        case None:
            break
        case Gif:
            .frameIndex = Math.min(idx, .numFrames)
            if .frameIndex < .cacheFrames.count
            {
                .frame      = .cacheFrames[.frameIndex]
                .dirtyFrame = true
            }

        case SpriteSheet:
            break
        }

        .sigFrameChanged.call(me)
    }

    // Set/Reset pause state
    mtd pause(state: bool)
    {
        .inPause = state
    }

    // Update timing and change frames
    mtd update(dt: f32)->bool
    {
        let wasDirty = .dirtyFrame
        .dirtyFrame = false

        if .inPause do
            return wasDirty

        var hasChanged = false
        .currentTime += dt
        while .currentTime > .playFreq
        {
            hasChanged = true
            .currentTime -= .playFreq
            .nextFrame()
        }

        return hasChanged or wasDirty
    }
}
