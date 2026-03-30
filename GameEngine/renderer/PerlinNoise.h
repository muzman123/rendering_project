#pragma once
#include <vector>
#include <cmath>

// Classic Perlin noise implementation.
// Ken Perlin invented this in 1983 for the movie Tron.
// It generates smooth, natural-looking random values
// by interpolating between random gradients at grid points.
//
// The key insight: pure random numbers are jagged (white noise).
// Perlin noise is smooth because it assigns random GRADIENTS
// to a grid and smoothly interpolates between them.
// The result looks like natural terrain, clouds, marble, etc.

class PerlinNoise {
public:
    // seed controls the randomness — same seed = same terrain every time
    PerlinNoise(unsigned int seed = 31285812597132);

    // Raw Perlin noise at a 2D point. Returns roughly -1 to +1.
    float noise(float x, float z) const;

    // Fractal Brownian Motion (fBM): layers multiple octaves of noise.
    // Each octave is higher frequency but lower amplitude.
    // This is what makes terrain look natural — big hills with
    // smaller bumps on top, with even smaller detail on those.
    //
    // octaves:     how many layers (6-8 is typical for terrain)
    // lacunarity:  how much the frequency increases each octave (usually 2.0)
    // persistence: how much the amplitude decreases each octave (usually 0.5)
    float fbm(float x, float z, int octaves = 6,
        float lacunarity = 10.0f, float persistence = 3.0f) const;

private:
    // Permutation table — a shuffled array of 0-255
    // Used to pick pseudo-random gradients at each grid point
    std::vector<int> m_perm;

    // Smooth interpolation (Perlin's improved version)
    // This is what makes the noise look smooth instead of jagged.
    // Regular linear interpolation creates visible grid lines.
    // This quintic curve (6t^5 - 15t^4 + 10t^3) has zero first
    // AND second derivatives at 0 and 1, so transitions are seamless.
    float fade(float t) const {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    float lerp(float a, float b, float t) const {
        return a + t * (b - a);
    }

    // Compute the gradient dot product at a grid corner.
    // 'hash' selects one of several gradient directions.
    // The dot product of the gradient with the distance vector
    // gives the noise contribution from this corner.
    float grad(int hash, float x, float z) const;
};