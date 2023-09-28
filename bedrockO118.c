#include <stdio.h>
#include <stdint.h>
#include "md5.h"

//typedefs and stuff
typedef struct {
    int x;
    int y;
    int z;
} Pos3d;

typedef struct {uint64_t high, low;} Xrng;
//end typedefs and stuff


//helper functions
static inline double lerp(double delta, double start, double end) {
    return start + delta * (end - start);
}

static inline double getLerpProgress(double value, double start, double end) {
    return (value - start) / (end - start);
}

static inline double lerpFromProgress(double lerpValue, double lerpStart, double lerpEnd, double start, double end) {
    return lerp(getLerpProgress(lerpValue, lerpStart, lerpEnd), start, end);
}

static int64_t hashCode(const int32_t x, const int32_t z)
{
    int64_t i = (int64_t)(int32_t)(3129871U * (uint32_t)x) ^ (int64_t)((uint64_t)z * 116129781ULL) ^ (int64_t)-60;
    i = i * i * 42317861ULL + i * 11ULL;
    return i >> 16;
}
//end helper functions


//rng functions
static inline uint64_t rotl64(uint64_t x, uint8_t b)
{
    return (x << b) | (x >> (64-b));
}

static inline uint64_t nextLong(Xrng *xr)
{
    uint64_t l = xr->low;
    uint64_t h = xr->high;
    uint64_t n = rotl64(l + h, 17) + l;
    h ^= l;
    xr->low = rotl64(l, 49) ^ h ^ (h << 21);
    xr->high = rotl64(h, 28);
    return n;
}

static inline uint64_t next(Xrng *xr, int bits) {
    return nextLong(xr) >> (64 - bits);
}

static inline float nextFloat(Xrng *xr)
{
    return (float)next(xr, 24) * 5.9604645E-8f;
}

static uint64_t nextSplitMix64Int(uint64_t seed) {
    seed = (seed ^ (seed >> 30)) * -4658895280553007687LL;
    seed = (seed ^ (seed >> 27)) * -7723592293110705685LL;
    return seed ^ (seed >> 31);
}

static void createXoroshiroSeed(Xrng *xr, uint64_t seed) {
    uint64_t l = seed ^ 0x6A09E667F3BCC909L;
    uint64_t m = l - 7046029254386353131L;
    xr->low = nextSplitMix64Int(l);
    xr->high = nextSplitMix64Int(m);
}

static void createRandomDeriver(Xrng *xr)
{
    uint64_t low;
    low = nextLong(xr);
    xr->high = nextLong(xr);
    xr->low = low;
}

void createRandom(Xrng* randomDeriver, Pos3d pos) {
    uint64_t l = hashCode(pos.x, pos.y, pos.z);
    uint64_t m = l ^ randomDeriver->low;
    randomDeriver->low = m;
}

void createRandomString(Xrng *xr, char *str)
{
    uint8_t result[16];
    md5String(str, result);
    Xrng md5Hash;
    uint64_t temp = 0;
    for(int i = 0; i < 8; i++)
    {
        temp <<= 8;
        temp |= result[i];
    }
    md5Hash.low = temp;
    temp = 0;
    for(int i = 8; i < 16; i++)
    {
        temp <<= 8;
        temp |= result[i];
    }
    md5Hash.high = temp;

    xr->low ^= md5Hash.low;
    xr->high ^= md5Hash.high;
}
//end rng functions


int isBedrock(Pos3d pos, double density, Xrng skipRng)
{
    createRandom(&skipRng, pos);
    return (double)nextFloat(&skipRng) < density;
}

int main()
{

    Xrng skipRng;
    Pos3d pos;
    pos.y = -60;

    double density = lerpFromProgress(pos.y, -64, -59, 1.0, 0.0);
    createXoroshiroSeed(&skipRng, 694201337ULL);
    createRandomDeriver(&skipRng);
    createRandomString(&skipRng, "minecraft:bedrock_floor");
    createRandomDeriver(&skipRng);


    for(pos.z = 0; pos.z < 10; pos.z++)
    {
        for(pos.x = 0; pos.x < 10; pos.x++)
        {
            if(isBedrock(pos, density, skipRng)) printf("# ");
            else printf("_ ");
        }
        printf("\n");
    }
}
