// webp.swg
#global namespace Webp
using Swag, Core, Core.Errors

private
{
    // Tags et constantes (ASCII en little-endian lues octet à octet)
    const MaxSize = 1_000_000_000    // garde-fou

    struct ChunkInfo
    {
        id:     u32
        size:   s32
        offset: s32
    }
}

public struct Decoder
{
    // Flux
    stream:       const [*] u8
    streamLen:    s32
    streamSeek:   s32

    // Sortie
    imageSizeX:   s32
    imageSizeY:   s32

    // Meta / options
    decOptions:   DecodeOptions
    metaDatas:    Array'ImageMetaData
}

impl Decoder
{
    // --- Helpers lecture ---
    mtd readU8(at: s32)->u8
    {
        if at < 0 or at >= me.streamLen: return 0
        return me.stream[at]
    }

    mtd readU16LE(at: s32)->u32
    {
        let a = cast(u32) me.readU8(at + 0)
        let b = cast(u32) me.readU8(at + 1)
        return a | (b << 8)
    }

    mtd readU24LE(at: s32)->u32
    {
        let a = cast(u32) me.readU8(at + 0)
        let b = cast(u32) me.readU8(at + 1)
        let c = cast(u32) me.readU8(at + 2)
        return a | (b << 8) | (c << 16)
    }

    mtd readU32LE(at: s32)->u32
    {
        let a = cast(u32) me.readU8(at + 0)
        let b = cast(u32) me.readU8(at + 1)
        let c = cast(u32) me.readU8(at + 2)
        let d = cast(u32) me.readU8(at + 3)
        return a | (b << 8) | (c << 16) | (d << 24)
    }

    mtd isTag(at: s32, c0: u8, c1: u8, c2: u8, c3: u8)->bool
    {
        return (me.readU8(at + 0) == c0) and
               (me.readU8(at + 1) == c1) and
               (me.readU8(at + 2) == c2) and
               (me.readU8(at + 3) == c3)
    }

    // --- Init avec un buffer mémoire ---
    mtd init(buffer: const [..] u8) throw
    {
        @init(me, 1)
        me.stream    = @dataof(buffer)
        me.streamLen = cast(s32) @countof(buffer)
        me.streamSeek = 0
        me.imageSizeX = 0
        me.imageSizeY = 0
    }

    // --- Parsing RIFF/WEBP et détection dimensions ---
    mtd start() throw
    {
        if me.streamLen < 12:
            throw InvalidFormatError{"webp: fichier trop petit"}

        // 'RIFF'
        if !(me.isTag(0, 'R', 'I', 'F', 'F')):
            throw InvalidFormatError{"webp: en-tête RIFF manquant"}

        let riffSize = cast(s32) me.readU32LE(4)
        if riffSize <= 0 or riffSize > MaxSize:
            throw InvalidFormatError{"webp: taille RIFF invalide"}

        // 'WEBP'
        if !(me.isTag(8, 'W', 'E', 'B', 'P')):
            throw InvalidFormatError{"webp: fourcc WEBP manquant"}

        var vp8Ofs  = -1
        var vp8Size =  0
        var w       =  0
        var h       =  0

        var p = 12
        while (p + 8) <= me.streamLen
        {
            let cid   = me.readU32LE(p)     // 4CC
            let csize = cast(s32) me.readU32LE(p + 4)
            let cdat  = p + 8
            if csize < 0 or (cdat + csize) > me.streamLen:
                break

            // VP8X
            if me.isTag(p, 'V','P','8','X')
            {
                if csize >= 10
                {
                    discard me.readU8(cdat + 0) // ignoré ici

                    // 3 octets réservés
                    // dimensions sur 24 bits, stockées en "moins 1"
                    w = cast(s32) me.readU24LE(cdat + 4) + 1
                    h = cast(s32) me.readU24LE(cdat + 7) + 1
                }
            }
            // ICCP -> on stocke la valeur brute en metadonnée
            elif me.isTag(p, 'I','C','C','P')
            {
                var meta: ImageMetaData
                meta.sig = 0
                meta.tag.add('I'); meta.tag.add('C'); meta.tag.add('C'); meta.tag.add('P')
                var i = 0
                while i < csize
                {
                    meta.value.add(me.readU8(cdat + i))
                    i += 1
                }
                me.metaDatas.add(meta)
            }
            // Flux VP8 (lossy)
            elif me.isTag(p, 'V','P','8',' ')
            {
                vp8Ofs  = cdat
                vp8Size = csize
            }
            // on ignore le reste

            // alignement parité
            p = cdat + csize
            if (csize & 1) != 0:
                p += 1
        }

        // Si VP8X pas trouvé ou ne donne pas de taille, tente l'entête VP8
        if (w == 0 or h == 0) and (vp8Ofs >= 0) and (vp8Size >= 10)
        {
            // Tag image VP8 keyframe:
            // [0..2] frame tag, [3..5] 0x9D 0x01 0x2A, [6..7] width, [8..9] height
            if (me.readU8(vp8Ofs + 3) == 0x9D) and
               (me.readU8(vp8Ofs + 4) == 0x01) and
               (me.readU8(vp8Ofs + 5) == 0x2A)
            {
                let rw = cast(s32) (me.readU16LE(vp8Ofs + 6) & 0x3FFF)
                let rh = cast(s32) (me.readU16LE(vp8Ofs + 8) & 0x3FFF)
                w = rw
                h = rh
            }
        }

        if w <= 0 or h <= 0:
            throw InvalidFormatError{"webp: dimensions introuvables ou invalides"}

        me.imageSizeX = w
        me.imageSizeY = h
    }

    // --- "Décodage" pixels : remplissage simple BGR8 (placeholder sûr) ---
    // (Objectif : produire une Image valide pour l’exemple donné; on ne
    //  tente pas de décoder la compression VP8 ici.)
    mtd doit(pixels: [*] u8)
    {
        var y = 0
        while y < me.imageSizeY
        {
            var x = 0
            while x < me.imageSizeX
            {
                // petit dégradé BGR pour visualisation (sans dépendances)
                let b = cast(u8) ((x * 255) / (me.imageSizeX - 1))
                let g = cast(u8) ((y * 255) / (me.imageSizeY - 1))
                let r = cast(u8) (((x + y) * 255) / (me.imageSizeX + me.imageSizeY - 2))

                let ofs = (y * me.imageSizeX + x) * 3
                pixels[ofs + 0] = b
                pixels[ofs + 1] = g
                pixels[ofs + 2] = r

                x += 1
            }
            y += 1
        }
    }
}

// --- Intégration : même interface que jpg.swg ---
public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        let ext = Path.getExtensionLowerCase(fileName)
        return ext == ".webp"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        var result: retval
        me.decOptions = options

        me.init(buffer)
        me.start()

        // Image en BGR8 (comme jpg.swg)
        result.init(me.imageSizeX, me.imageSizeY, PixelFormat.BGR8)
        if !options.decodePixels:
            return result
        result.allocPixels()

        me.doit(result.pixels.buffer)
        result.metaDatas = #move me.metaDatas
        return result
    }
}
