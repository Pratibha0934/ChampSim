#include "champsim.h"
#include "block.h"

// Defining set types
#define VERY_COLD 0
#define COLD 1
#define VERY_HOT 2
#define HOT 3
#define NEUTRAL 4

// class to organise and distribute blocks among sets
class CACHE_ORGANISER
{
    pair<uint32_t, uint64_t> *temperature; // stores temperature of each set
    int32_t *helper;                       // stores helper set of each set
    int32_t *parent;                       // stores the parent set of a helper set
    uint8_t *set_type;                     // stores the type of set for each set
    uint32_t num_sets, num_ways;           // stores sets and ways of the cache
    BLOCK **block;                         // pointer to cache blocks
    bool warmup_state;                     // stores warmup state of the program

    // function for sorting *temperature
    static bool sortbysec(const pair<uint32_t, uint64_t> &a, const pair<uint32_t, uint64_t> &b)
    {
        return (a.second > b.second);
    }

    // Function to allot helpers
    void allot_helpers()
    {
        // sort sets based on temperature
        sort(temperature, temperature + num_sets, sortbysec);

        for (uint32_t set = 0; set < num_sets; set++)
        {
            // linking hot sets to cold sets -> Assigning helpers
            if (set < num_sets / 2)
            {
                set_type[set] = (set < num_sets / 4) ? VERY_HOT : HOT;

                uint32_t helper_set_index = num_sets - set - 1;

                // assign helper set
                helper[temperature[set].first] = temperature[helper_set_index].first;

                // assign parent set
                parent[temperature[helper_set_index].first] = temperature[set].first;
            }

            // Give 25% blocks of cold sets to hot sets
            // and update lru values
            if (set >= num_sets / 2 && set < (num_sets * 3) / 4)
            {
                set_type[set] = COLD;
                for (uint32_t way = 0; way < num_ways / 4; way++)
                {
                    block[set][way].foreign = 1;
                    block[set][way].lru = way + num_ways;
                }

                for (uint32_t way = num_ways / 4; way < num_ways; way++)
                {
                    block[set][way].lru = way - (num_ways / 4);
                }
            }

            // Give 50% blocks of very cold sets to very hot sets
            // and update lru values
            if (set >= (num_sets * 3) / 4 && set < num_sets)
            {
                set_type[set] = VERY_COLD;
                for (uint32_t way = 0; way < num_ways / 2; way++)
                {
                    block[set][way].foreign = 1;
                    block[set][way].lru = way + num_ways;
                }

                for (uint32_t way = num_ways / 2; way < num_ways; way++)
                {
                    block[set][way].lru = way - (num_ways / 2);
                }
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

    // Getter function to get set type
    uint8_t get_set_type(uint32_t set)
    {
        return set_type[set];
    }

    // Constructor
    CACHE_ORGANISER(uint32_t num_sets = 2048, uint32_t num_ways = 16, BLOCK **block = NULL)
    {
        // initialise variables
        this->num_sets = num_sets;
        this->num_ways = num_ways;
        this->warmup_state = true;
        this->block = block;
    }

    // Initialisation function
    void initialise_organiser()
    {
        // allocating memory
        temperature = new pair<uint32_t, uint64_t>[num_sets];
        helper = new int32_t[num_sets];
        parent = new int32_t[num_sets];
        set_type = new uint8_t[num_sets];

        // initialising values
        for (uint32_t set = 0; set < num_sets; set++)
        {
            temperature[set].first = set;
            temperature[set].second = 0;
            helper[set] = -1;
            parent[set] = -1;
            set_type[set] = NEUTRAL;
        }
    }
};