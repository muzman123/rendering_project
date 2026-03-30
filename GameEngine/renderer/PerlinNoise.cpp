#include "PerlinNoise.h"
#include <algorithm>
#include <numeric>
#include <random>

PerlinNoise::PerlinNoise(unsigned int seed) {
    // Create array 0-255, shuffle it with the seed
    m_perm.resize(512);
    std::vector<int> p(256);
    std::iota(p.begin(), p.end(), 0);  // Fill with 0, 1, 2, ... 255

    std::mt19937 rng(seed);
    std::shuffle(p.begin(), p.end(), rng);

    // Duplicate so we don't need to wrap with modulo
    for (int i = 0; i < 256; i++) {
        m_perm[i] = p[i];
        m_perm[256 + i] = p[i];
    }
}

float PerlinNoise::grad(int hash, float x, float z) const {
    // Use the low 4 bits to select one of 8 gradient directions.
    // These gradients point to the edges of a square:
    // (1,0), (-1,0), (0,1), (0,-1), (1,1), (-1,1), (1,-1), (-1,-1)
    // The dot product with the distance vector gives a smooth
    // contribution that varies naturally with position.
    switch (hash & 7) {
    case 0: return  x + z;
    case 1: return -x + z;
    case 2: return  x - z;
    case 3: return -x - z;
    case 4: return  x;
    case 5: return -x;
    case 6: return  z;
    case 7: return -z;
    default: return 0;
    }
}

float PerlinNoise::noise(float x, float z) const {
    // Find which grid cell we're in
    int xi = (int)std::floor(x) & 255;
    int zi = (int)std::floor(z) & 255;

    // Position within the cell (0 to 1)
    float xf = x - std::floor(x);
    float zf = z - std::floor(z);

    // Smooth the position with the fade function
    float u = fade(xf);
    float v = fade(zf);

    // Hash the four corners of the grid cell to get gradient indices
    int aa = m_perm[m_perm[xi] + zi];
    int ab = m_perm[m_perm[xi] + zi + 1];
    int ba = m_perm[m_perm[xi + 1] + zi];
    int bb = m_perm[m_perm[xi + 1] + zi + 1];

    // Compute gradient dot products at each corner and interpolate
    float x1 = lerp(grad(aa, xf, zf), grad(ba, xf - 1, zf), u);
    float x2 = lerp(grad(ab, xf, zf - 1), grad(bb, xf - 1, zf - 1), u);

    return lerp(x1, x2, v);
}

float PerlinNoise::fbm(float x, float z, int octaves,
    float lacunarity, float persistence) const {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;  // For normalizing the result to 0-1

    for (int i = 0; i < octaves; i++) {
        total += noise(x * frequency, z * frequency) * amplitude;
        maxValue += amplitude;

        // Each octave: double the frequency, halve the amplitude
        frequency *= lacunarity;
        amplitude *= persistence;
    }

    // Normalize to roughly 0 to 1
    return (total / maxValue) * 0.5f + 0.5f;
}