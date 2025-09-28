using Core

impl Image
{
    // Fill the image with a noise.
    public mtd noise(noise: *Noise.FastNoise)
    {
        .visitPixels(noise)
        {
            let params = cast(*Noise.FastNoise) userData
            let v      = params.noise(x, y)
            let light  = cast(u8) Math.map(v, -1.0, 1.0, 0, 255.0)
            pix[0] = light
            pix[1] = light
            pix[2] = light
        }
    }
}
