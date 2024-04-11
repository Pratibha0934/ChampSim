#include "champsim.h"
#include "block.h"

// class to organise and distribute blocks among sets
class CACHE_ORGANISER
{
    pair<uint32_t, uint64_t> *temperature; // stores temperature of each set
    int32_t *helper;                       // stores helper set of each set
    uint32_t num_sets, num_ways;           // stores sets and ways of the cache
    BLOCK **block;                         // pointer to cache blocks
    bool warmup_state;                     // stores warmup state of the program

    // Initialisation function
    void initialise_organiser()
    {
        // allocating memory
        temperature = new pair<uint32_t, uint64_t>[num_sets];
        helper = new int32_t[num_sets];

        // initialising values
        for (uint32_t i = 0; i < num_sets; i++)
        {
            temperature[i].first = i;
            temperature[i].second = 0;
            helper[i] = -1;
        }
    }

    // function for sorting *temperature
    bool sortbysec(const pair<uint32_t, uint64_t> &a, const pair<uint32_t, uint64_t> &b)
    {
        return (a.second > b.second);
    }

    // Function to allot helpers
    void allot_helpers()
    {
        // sort sets based on temperature
        sort(temperature, temperature + num_sets, sortbysec);

        for (uint32_t i = 0; i < num_sets / 2; i++)
        {
            // assign helper set
            helper[temperature[i].first] = temperature[num_sets - i - 1].first;

            // set only half of the blocks in the helper set to cold
            // remaining will remain cold to be used by hot set
            for (uint32_t j = num_ways / 2; j < num_ways; j++)
            {
                block[num_sets - i - 1][j].hot = 0;
            }
        }
    }

public:
    // Function to update temperature
    void update_temperature(uint32_t set, uint32_t cpu)
    {
        if (warmup_state) // check warmup state
        {
            temperature[set].second = temperature[set].second + 1;

            if (warmup_complete[cpu])
            {
                // update state and allot helpers
                warmup_state = false;
                allot_helpers();
            }
        }
    }

    // Getter function to get helper set
    uint32_t get_helper_set(uint32_t set)
    {
        return helper[set];
    }

    // Constructor
    CACHE_ORGANISER(uint32_t num_sets = 2048, uint32_t num_ways = 16, BLOCK **block = NULL)
    {
        // initialise variables
        this->num_sets = num_sets;
        this->num_ways = num_ways;
        this->warmup_state = true;
        this->block = block;

        initialise_organiser();
    }

    // Destructor
    ~CACHE_ORGANISER()
    {
        delete temperature;
        delete helper;
    }
};