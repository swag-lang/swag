#global namespace Random

#[Swag.Tls]
private var g_SharedRng: Rng'MWC

// Get the default per thread random generator
public func shared() => &g_SharedRng
