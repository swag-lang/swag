/*
    :LICENCE
    This code is based on FastNoise by Jordan Peck (https://github.com/Auburn/FastNoiseLite/blob/FastNoise-Legacy/FastNoise.cpp)
    See LICENCE.md for the corresponding licence.
*/

#global namespace Noise
#global #[Swag.Safety("", false)]

public struct FastNoise
{
    seed:               s32 = 1337

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

        loop i: 1 until octaves
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
        var xi  = x * frequency
        var yi  = y * frequency
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
}
