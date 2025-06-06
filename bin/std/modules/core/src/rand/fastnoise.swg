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
    ValueFractalFBM
    ValueFractalBillow
    ValueFractalRigidMulti
}

public struct FastNoise
{
    kind                = FastNoiseKind.Perlin
    seed                = 1337
    octaves             = 3                        // The number of layers of noise. More octaves create a more varied look but are also more resource-intensive to calculate.
    frequency           = 0.01                     // The period in which we sample the noise. A higher frequency results in more frequent noise change.
    lacunarity          = 2.0                      // The rate of change of the frequency for each successive octave. A 'lacunarity' value of 1 results in each octave having the same frequency.
    gain                = 0.5                      // The scaling factor applied to each octave. 'gain' is only used when 'octaves' is set to a value higher than 1.

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

        for i in octaves - 1
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

        for i in octaves
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

        for i in octaves
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

        for i in octaves
        {
            xi, yi *= lacunarity
            amp *= gain
            sum += (1 - Math.abs(perlin(xi, yi, seed))) * amp
        }

        return sum * fractalBounding
    }

    public mtd const valueFractalFBM(x, y: f32)->f32
    {
        var xi  = x
        var yi  = y
        var sum = Noise.value(xi, yi, seed)
        var amp = 1.0

        for i in octaves
        {
            xi, yi *= lacunarity
            amp *= gain
            sum += Noise.value(xi, yi, seed) * amp
        }

        return sum * fractalBounding
    }

    public mtd const valueFractalBillow(x, y: f32)->f32
    {
        var xi  = x
        var yi  = y
        var sum = Math.abs(Noise.value(xi, yi, seed)) * 2 - 1
        var amp = 1.0

        for i in octaves
        {
            xi, yi *= lacunarity
            amp *= gain
            sum += (Math.abs(Noise.value(xi, yi, seed)) * 2 - 1) * amp
        }

        return sum * fractalBounding
    }

    public mtd const valueFractalRigidMulti(x, y: f32)->f32
    {
        var xi  = x
        var yi  = y
        var sum = 1 - Math.abs(Noise.value(xi, yi, seed))
        var amp = 1.0

        for i in octaves
        {
            xi, yi *= lacunarity
            amp *= gain
            sum += (1 - Math.abs(Noise.value(xi, yi, seed))) * amp
        }

        return sum * fractalBounding
    }

    // Get a noise result.
    public mtd const noise(x, y: f32)->f32
    {
        var result: f32 = undefined

        let xi = x * frequency
        let yi = y * frequency

        #[Swag.Complete]
        switch kind
        {
        case Perlin:
            result = Noise.perlin(xi, yi, seed)
        case PerlinFractalFBM:
            result = perlinFractalFBM(xi, yi)
        case PerlinFractalBillow:
            result = perlinFractalBillow(xi, yi)
        case PerlinFractalRigidMulti:
            result = perlinFractalRigidMulti(xi, yi)
        case Value:
            result = Noise.value(xi, yi, seed)
        case ValueFractalFBM:
            result = valueFractalFBM(xi, yi)
        case ValueFractalBillow:
            result = valueFractalBillow(xi, yi)
        case ValueFractalRigidMulti:
            result = valueFractalRigidMulti(xi, yi)
        }

        return Math.clamp(result, -1.0, 1.0)
    }
}
