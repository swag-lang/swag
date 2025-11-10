const Pi         = 3.1415
const solar_mass = 4 * Pi * Pi
const year       = 365.24'f64

struct Planet
{
    x:        f64
    y:        f64
    z:        f64
    vx:       f64
    vy:       f64
    vz:       f64
    mass:     f64
}

const solar_bodies: [?] Planet = [
    // Sun
    {
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        solar_mass},
    // Jupiter
    {
        4.84143144246472090e+00,
        -1.16032004402742839e+00,
        -1.03622044471123109e-01, 1.66007664274403694e-03 * year, 7.69901118419740425e-03 * year, -6.90460016972063023e-05 * year, 9.54791938424326609e-04 * solar_mass},
    // Saturn
    {
        8.34336671824457987e+00,
        4.12479856412430479e+00,
        -4.03523417114321381e-01, -2.76742510726862411e-03 * year, 4.99852801234917238e-03 * year, 2.30417297573763929e-05 * year, 2.85885980666130812e-04 * solar_mass},
    // Uranus
    {
        1.28943695621391310e+01,
        -1.51111514016986312e+01,
        -2.23307578892655734e-01, 2.96460137564761618e-03 * year, 2.37847173959480950e-03 * year, -2.96589568540237556e-05 * year, 4.36624404335156298e-05 * solar_mass},
    // Neptune
    {
        1.53796971148509165e+01,
        -2.59193146099879641e+01,
        1.79258772950371181e-01, 2.68067772490389322e-03 * year, 1.62824170038242295e-03 * year, -9.51592254519715870e-05 * year, 5.15138902046611451e-05 * solar_mass}]

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
func energy(bodies: [*] Planet)->f64
{
    //return bodies.vx * bodies.vx + bodies.vy
    var e: f64 = 0.0

    //foreach bi, i in bodies[..@countof(bodies)-2]
    for var i = 0; i < 5; i += 1
    {
        let bi = &bodies[i]
        e += 0.5 * (bi.vx * bi.vx + bi.vy * bi.vy + bi.vz * bi.vz) * bi.mass

        //foreach bj in bodies[i+1..]
        for var j = i + 1; j < 5; j += 1
        {
            let bj   = &bodies[j]
            let dx   = bi.x - bj.x
            let dy   = bi.y - bj.y
            let dz   = bi.z - bj.z
            let dist = @sqrt(dx * dx + dy * dy + dz * dz)
            e -= bi.mass * bj.mass / dist
        }
    }

    return e
}

func advance(bodies: [..] Planet, dt: f64, steps: u64)
{
    for s in steps
    {
        for var i = 0; i < 5; i += 1 //for i in 3
        //foreach &bi, j in  bodies[..@countof(bodies)-2]
        {
            let bi = &bodies[i]
            var vx = bi.vx
            var vy = bi.vy
            var vz = bi.vz

            for var j = i + 1; j < 5; j += 1 //foreach &bj in bodies[j+1 ..]
            {
                let bj = &bodies[j]

                let dx = bi.x - bj.x
                let dy = bi.y - bj.y
                let dz = bi.z - bj.z

                let dsq = dx * dx + dy * dy + dz * dz
                let dst = @sqrt(dsq)
                let mag = dt / (dsq * dst)
                let mi  = bi.mass

                vx -= dx * bj.mass * mag
                vy -= dy * bj.mass * mag
                vz -= dz * bj.mass * mag

                bj.vx += dx * mi * mag
                bj.vy += dy * mi * mag
                bj.vz += dz * mi * mag
            }

            bi.vx = vx
            bi.vy = vy
            bi.vz = vz

            bi.x += dt * vx
            bi.y += dt * vy
            bi.z += dt * vz
        }
    }
}

func offset_momentum(bodies: [..] Planet)
{
    var px: f64 = 0.0
    var py: f64 = 0.0
    var pz: f64 = 0.0

    foreach b in bodies
    {
        px -= b.vx * b.mass
        py -= b.vy * b.mass
        pz -= b.vz * b.mass
    }

    let sun = &bodies[0]
    sun.vx = px / solar_mass
    sun.vy = py / solar_mass
    sun.vz = pz / solar_mass
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    //Time.Stopwatch.scopeMeasure()

    var bodies = solar_bodies
    offset_momentum(bodies)
    var ret = energy(bodies)
    //@print(ret, "\n")

    //var n = 5_000_000'u64
    let n = 5'u64
    advance(bodies, 0.01, n)
    ret = energy(bodies)
    //@print(ret, "\n")
}
