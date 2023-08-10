#include <stdio.h>
#include <stdint.h>
#define MASK48 281474976710655ULL

static inline double lerp(double delta, double start, double end) {
    return start + delta * (end - start);
}

static inline double getLerpProgress(double value, double start, double end) {
    return (value - start) / (end - start);
}

static inline double lerpFromProgress(double lerpValue, double lerpStart, double lerpEnd, double start, double end) {
    return lerp(getLerpProgress(lerpValue, lerpStart, lerpEnd), start, end);
}

//legacy java random
void setSeed(uint64_t *seed, uint64_t value)
{
    *seed = (value ^ 0x5deece66d) & ((1ULL << 48) - 1);
}

int next(uint64_t *seed, const int bits)
{
    *seed = (*seed * 0x5deece66d + 0xb) & ((1ULL << 48) - 1);
    return (int) ((int64_t)*seed >> (48 - bits));
}

static inline float nextFloat(uint64_t *seed)
{
    return next(seed, 24) / (float) (1 << 24);
}

uint64_t nextLong(uint64_t *seed)
{
    return ((uint64_t) next(seed, 32) << 32) + next(seed, 32);
}

int64_t hashCode(int32_t x, int32_t y, int32_t z) {
    uint64_t l = (uint64_t)(x * 3129871) ^ ((uint64_t)z * 116129781ULL) ^ (uint64_t)y;
    l = l * l * 42317861ULL + l * 11ULL;
    return l >> 16;
}

/*
long hashCode(int x, int y, int z)
{
    long i = (long)(x * 3129871) ^ (long)z * 116129781ULL ^ (long)y;
    i = i * i * 42317861ULL + i * 11ULL;
    i >>= 16;
    return i;
}
*/

void atFunction(uint64_t *state, uint64_t worldSeed, int32_t x, int32_t y, int32_t z)
{
    *state = hashCode(x, y, z);
    *state ^= worldSeed;
    setSeed(state, *state);
}

int stringHashCode(const char *str) {
    int hash = 0;
    int i = 0;

    while(str[i] != 0) {
        hash = 31 * hash + str[i];
        i++;
    }
    return hash;
}

int isBedrock(uint64_t worldSeed, int32_t x, int32_t y, int32_t z)
{
    uint64_t state;
    double density;


    //get roof/floor seed
    worldSeed &= MASK48;
    setSeed(&state, worldSeed);
    state = nextLong(&state) & MASK48;
    //cases that will never be true
    if(y < 0 || (y > 4 && y < 123) || y > 127) return 0;
    if(y < 5)
    {
        density = lerpFromProgress(y, 0, 5, 1.0, 0.0);
        state ^= stringHashCode("minecraft:bedrock_floor");
    }
    else
    {
        density = lerpFromProgress(y, 127, 127-5, 1.0, 0.0);
        state ^= stringHashCode("minecraft:bedrock_roof");
    }
    setSeed(&state, state);
    state = nextLong(&state) & MASK48;
    state ^= hashCode(x, y, z);
    setSeed(&state, state);

    if(y < 5)
    {
        return (double)nextFloat(&state) < density;
    }
    else
    {
        return (double)nextFloat(&state) > (1 - density);
    }
}

int main()
{
    for(int j = 0; j < 8; j++)
    {
        for(int i = 0; i < 8; i++)
        {
            if(isBedrock(1442567685227760047ULL, j, 123, i)) printf("# ");
            else printf("_ ");
        }
        printf("\n");
    }
    return 0;
}