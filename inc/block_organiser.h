#include "champsim.h"

class BLOCK_ORGANISER
{
    map<string, uint64_t> *block_map;
    uint32_t sets;

public:
    // Initialising valuse of block_map
    void initialise_block_map()
    {
        for (size_t i = 0; i < sets; i++)
        {
            block_map[i]["set"] = i;
            block_map[i]["temperature"] = 0;
        }
    }

    // Update state
    void update_state(uint32_t set, BLOCK **cache)
    {
    }

    // Parameterised constructor
    BLOCK_ORGANISER(uint32_t sets)
    {
        this->sets = sets;
        block_map = new map<string, uint64_t>[sets];
    }

    // Default onstructor
    BLOCK_ORGANISER()
    {
        this->sets = 2048;
        block_map = new map<string, uint64_t>[2048];
    }
};