#global marked
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
        if me.frameIndex == me.numFrames - 1
        {
            if me.mustLoop:
                me.frameIndex = 0
            else:
                return
        }
        else:
            me.frameIndex += 1

        #[Swag.Complete]
        switch me.source
        {
        case None:
            break
        case SpriteSheet:
            break

        case Gif:
            if me.frameIndex < me.cacheFrames.count
            {
                me.frame = me.cacheFrames[me.frameIndex]
            }
            elif assume me.gif.nextFrame(&me.frame) == false
            {
                if me.mustLoop
                {
                    me.gif.rewind()
                    me.frameIndex = 0
                    discard assume me.gif.nextFrame(&me.frame)
                }
            }
            elif me.enableCache
            {
                me.cacheFrames.add(me.frame)
            }
        }

        me.sigFrameChanged.call(me)
    }
}

impl Movie
{
    mtd clear()
    {
        me.cacheFrames.clear()
        me.bytes.clear()
        me.source = .None
        me.frame.release()
        me.numFrames   = 0
        me.frameIndex  = 0
        me.currentTime = 0
    }

    // Associate an image list
    #[Swag.Overload]
    mtd set(imageList: ImageList)
    {
        me.clear()
        me.source    = .SpriteSheet
        me.imgList   = imageList
        me.numFrames = me.imgList.totalFrames
    }

    // Associate a filename
    #[Swag.Overload]
    mtd set(fileName: string) throw
    {
        me.clear()
        if me.gif.IImageDecoder.canDecode(fileName)
        {
            me.source = .Gif
            me.bytes  = File.readAllBytes(fileName)
            me.gif.init(me.bytes.toSlice(), true)
            me.numFrames = me.gif.getNumFrames()
            me.gif.getFrame(&me.frame)
            discard me.gif.nextFrame(&me.frame)
            if me.enableCache:
                me.cacheFrames.add(me.frame)
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
        switch me.source
        {
        case None:
            return false
        case SpriteSheet:
            return me.imgList.texture.valid
        case Gif:
            return me.frame.pixels.buffer != null
        }

        return false
    }

    // Set the current video frame
    mtd setFrameIndex(idx: s32)
    {
        if idx == me.frameIndex:
            return
        me.frameIndex = Math.min(idx, me.numFrames)

        #[Swag.Complete]
        switch me.source
        {
        case None:
            break
        case Gif:
            me.frameIndex = Math.min(idx, me.numFrames)
            if me.frameIndex < me.cacheFrames.count
            {
                me.frame      = me.cacheFrames[me.frameIndex]
                me.dirtyFrame = true
            }

        case SpriteSheet:
            break
        }

        me.sigFrameChanged.call(me)
    }

    // Set/Reset pause state
    mtd pause(state: bool)
    {
        me.inPause = state
    }

    // Update timing and change frames
    mtd update(dt: f32)->bool
    {
        let wasDirty = me.dirtyFrame
        me.dirtyFrame = false

        if me.inPause:
            return wasDirty

        var hasChanged = false
        me.currentTime += dt
        while me.currentTime > me.playFreq
        {
            hasChanged = true
            me.currentTime -= me.playFreq
            me.nextFrame()
        }

        return hasChanged or wasDirty
    }
}
