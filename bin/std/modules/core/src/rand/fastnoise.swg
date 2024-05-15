/*
    :LICENCE
    This code is based on FastNoise by Jordan Peck (https://github.com/Auburn/FastNoiseLite/tree/FastNoise-Legacy)
    See LICENCE.md for the corresponding licence.
*/

#global namespace Noise
#global #[Swag.Safety("", false)]

public enum FastNoiseKind
{
    Perlin
    PerlinFractalFBM
    PerlinFractalBillow
    PerlinFractalRigidMulti
    Value
}

public struct FastNoise
{
    seed:               s32 = 1337
    kind                = FastNoiseKind.Perlin

    octaves:            s32 = 3
    frequency           = 0.01
    lacunarity          = 2.0
    gain                = 0.5

    fractalBounding     = 1.0
}

impl FastNoise
{
    mtd calculateFractalBounding()
    {
        if octaves == 1
        {
            fractalBounding = 1.0
            return
        }

        var amp        = gain
        var ampFractal = 1.0

        loop i: octaves - 1
        {
            ampFractal += amp
            amp *= gain
        }

        fractalBounding = 1.0 / ampFractal
    }

    // Update internal values.
    // Should be called each time an internal parameter has been changed.
    public mtd update()
    {
        calculateFractalBounding()
    }

    public mtd const perlinFractalFBM(x, y: f32)->f32
    {
        var xi  = x
        var yi  = y
        var sum = perlin(xi, yi, seed)
        var amp = 1.0

        loop i: octaves
        {
            xi, yi *= lacunarity
            amp *= gain
            sum += perlin(xi, yi, seed) * amp
        }

        return sum * fractalBounding
    }

    public mtd const perlinFractalBillow(x, y: f32)->f32
    {
        var xi  = x
        var yi  = y
        var sum = Math.abs(perlin(xi, yi, seed)) * 2 - 1
        var amp = 1.0

        loop i: octaves
        {
            xi, yi *= lacunarity
            amp *= gain
            sum += (Math.abs(perlin(xi, yi, seed)) * 2 - 1) * amp
        }

        return sum * fractalBounding
    }

    public mtd const perlinFractalRigidMulti(x, y: f32)->f32
    {
        var xi  = x
        var yi  = y
        var sum = 1 - Math.abs(perlin(xi, yi, seed))
        var amp = 1.0

        loop i: octaves
        {
            xi, yi *= lacunarity
            amp *= gain
            sum += (1 - Math.abs(perlin(xi, yi, seed))) * amp
        }

        return sum * fractalBounding
    }

    // Get a noise result.
    public mtd const noise(x, y: f32)->f32
    {
        var result: f32 = undefined

        var xi = x * frequency
        var yi = y * frequency

        #[Swag.Complete]
        switch kind
        {
            case Perlin:
                result = Noise.perlin(xi, yi)
            case PerlinFractalFBM:
                result = perlinFractalFBM(xi, yi)
            case PerlinFractalBillow:
                result = perlinFractalBillow(xi, yi)
            case PerlinFractalRigidMulti:
                result = perlinFractalRigidMulti(xi, yi)
            case Value:
                result = Noise.value(xi, yi)
        }

        return Math.clamp(result, -1.0, 1.0)
    }
}
