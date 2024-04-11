#include "champsim.h"
#include "block.h"

// class to organise and distribute blocks among sets
class CACHE_ORGANISER
{
    pair<uint32_t, uint64_t> *temperature; // stores temperature of each set
    int32_t *helper;                       // stores helper set of each set
    int32_t *parent;                       // stores the parent set of a helper set
    uint32_t num_sets, num_ways;           // stores sets and ways of the cache
    BLOCK **block;                         // pointer to cache blocks
    bool warmup_state;                     // stores warmup state of the program

    // Initialisation function
    void initialise_organiser()
    {
        // allocating memory
        temperature = new pair<uint32_t, uint64_t>[num_sets];
        helper = new int32_t[num_sets];
        parent = new int32_t[num_sets];

        // initialising values
        for (uint32_t i = 0; i < num_sets; i++)
        {
            temperature[i].first = i;
            temperature[i].second = 0;
            helper[i] = -1;
            parent[i] = -1;
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
            uint32_t helper_set_index = num_sets - i - 1;

            // assign helper set
            helper[temperature[i].first] = temperature[helper_set_index].first;

            // assign parent set
            parent[temperature[helper_set_index].first] = temperature[i].first;

            for (uint32_t j = num_ways / 2; j < num_ways; j++)
            {
                // set only half of the blocks in the helper set to cold
                // remaining will remain cold to be used by hot set
                block[helper_set_index][j].hot = 0;

                // update lru value of cold blocks
                block[helper_set_index][j].lru = j - (num_ways / 2);
            }

            // update lru value of hot blocks of helper set
            for (size_t j = 0; j < num_ways / 2; j++)
            {
                block[helper_set_index][j].lru = j + num_ways;
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
    int32_t get_helper_set(uint32_t set)
    {
        return helper[set];
    }

    // Getter function to get parent set
    int32_t get_parent_set(uint32_t set)
    {
        return parent[set];
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