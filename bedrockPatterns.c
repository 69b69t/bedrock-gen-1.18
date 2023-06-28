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

static uint64_t hashCode(int x, int y, int z) {
    int64_t l = (int64_t)(x * 3129871) ^ ((int64_t)z * 116129781LL) ^ (int64_t)y;
    l = l * l * 42317861L + l * 11LL;
    return l >> 16;
}
//end helper functions


//rng functions
static inline uint64_t rotl64(uint64_t x, uint8_t b)
{
    return (x << b) | (x >> (64-b));
}

static inline uint64_t nextLong(Xrng *xr) {
    uint64_t l = xr->low;
    uint64_t m = xr->high;
    uint64_t n = rotl64(l + m, 17) + l;
    xr->low = rotl64(l, 49) ^ (m ^= l) ^ m << 21;
    xr->high = rotl64(m, 28);
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


int isBedrock(Pos3d pos) {
        if (pos.y == -64) {
            return 1;
        } else if (pos.y >= -59) {
            return 0;
        } else {
        double var2x = lerpFromProgress(pos.y, -64, -59, 1.0, 0.0);

        Xrng randomDeriver;

        createXoroshiroSeed(&randomDeriver, 3773983928257503317ULL);
        createRandomDeriver(&randomDeriver);
        createRandomString(&randomDeriver, "minecraft:bedrock_floor");
        createRandomDeriver(&randomDeriver);

        createRandom(&randomDeriver, pos);
        return (double)nextFloat(&randomDeriver) < var2x;
    }
}

int main()
{
    Xrng randomDeriver;

    Pos3d pos;
    pos.y = -60;

    for(pos.z = -15; pos.z < 15; pos.z++)
    {
        for(pos.x = -15; pos.x < 15; pos.x++)
        {
            if(isBedrock(pos))
            {
                printf(" #");
            }
            else
            {
                printf(" _");
            }
        }
        printf("\n");
    }
}
