#include "cache.h"

// initialize replacement state
void CACHE::llc_initialize_replacement()
{
}

// find replacement victim
pair<uint32_t, uint32_t> CACHE::llc_find_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    pair<uint32_t, int32_t> victim;     // <set, way>
    pair<uint32_t, uint32_t> victim_lru; // <set, way>

    if (warmup_complete[cpu])
    {
        int32_t helper = cache_organiser.get_helper_set(set);
        uint32_t way;

        if (helper == -1) // For cold set
        {
            victim.first = set;

            // checking if any block is invalid
            for (way = NUM_WAY / 2; way < NUM_WAY; way++)
            {
                if (block[set][way].valid == false)
                {
                    victim.second = way;
                    break;
                }
            }

            // For LRU victim
            if (way == NUM_WAY)
            {
                for (way = NUM_WAY / 2; way < NUM_WAY; way++)
                {
                    if (block[set][way].lru == (NUM_WAY / 2) - 1)
                    {
                        victim.second = way;
                        break;
                    }
                }
            }

            if (way == NUM_WAY)
            {
                cerr << "[" << NAME << "] " << __func__ << " no victim! set: " << set << endl;
                assert(0);
            }

            return victim;
        }
        else // For hot set
        {
            victim.first = set;
            victim.second = -1;

            // checking if any block is invalid in main set
            for (way = 0; way < NUM_WAY; way++)
            {
                if (block[set][way].valid == false)
                {
                    victim.second = way;
                    break;
                }

                // lru victim
                if (block[set][way].lru == NUM_WAY + (NUM_WAY / 2) - 1)
                {
                    victim_lru.first = set;
                    victim_lru.second = way;
                }
            }

            // Searching in helper set
            if (way == NUM_WAY)
            {
                for (way = 0; way < NUM_WAY / 2; way++)
                {
                    if (block[helper][way].valid == false)
                    {
                        victim.second = way;
                        break;
                    }
                    // lru victim
                    if (block[set][way].lru == NUM_WAY + (NUM_WAY / 2) - 1)
                    {
                        victim_lru.first = helper;
                        victim_lru.second = way;
                    }
                }
            }

            if (victim.second == -1)
            {
                return victim_lru;
            }

            return victim;
        }
    }
    else
    {
        victim.first = set;
        victim.second = lru_victim(cpu, instr_id, set, current_set, ip, full_addr, type);
        return victim;
    }
}

// called on every cache hit and cache fill
void CACHE::llc_update_replacement_state(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    string TYPE_NAME;
    if (type == LOAD)
        TYPE_NAME = "LOAD";
    else if (type == RFO)
        TYPE_NAME = "RFO";
    else if (type == PREFETCH)
        TYPE_NAME = "PF";
    else if (type == WRITEBACK)
        TYPE_NAME = "WB";
    else
        assert(0);

    if (hit)
        TYPE_NAME += "_HIT";
    else
        TYPE_NAME += "_MISS";

    if ((type == WRITEBACK) && ip)
        assert(0);

    // uncomment this line to see the LLC accesses
    // cout << "CPU: " << cpu << "  LLC " << setw(9) << TYPE_NAME << " set: " << setw(5) << set << " way: " << setw(2) << way;
    // cout << hex << " paddr: " << setw(12) << paddr << " ip: " << setw(8) << ip << " victim_addr: " << victim_addr << dec << endl;

    // baseline LRU
    if (hit && (type == WRITEBACK)) // writeback hit does not update LRU state
        return;

    cache_organiser.update_temperature(set, cpu);

    if (warmup_complete[cpu]) // after warmup
    {
        // for a cold set
        if (block[set][way].hot == 0)
        {
            for (size_t i = 0; i < NUM_WAY; i++)
            {
                if (block[set][i].lru < block[set][way].lru && block[set][i].hot == 0)
                {
                    block[set][i].lru++;
                }
            }
            block[set][way].lru = 0;
        }
        else // for hot set
        {
            for (size_t i = 0; i < NUM_WAY; i++)
            {
                if (block[set][i].lru < block[set][way].lru && block[set][i].hot == 1)
                {
                    block[set][i].lru++;
                }
            }

            // get helper/parent set if available
            int32_t next_set = cache_organiser.get_helper_set(set);

            set = (next_set != -1) ? next_set : cache_organiser.get_parent_set(set);

            for (size_t i = 0; i < NUM_WAY; i++)
            {
                if (block[set][i].lru < block[set][way].lru && block[set][i].hot == 1)
                {
                    block[set][i].lru++;
                }
            }

            block[set][way].lru = 0;
        }
    }
    else // before warmup
    {
        return lru_update(set, way);
    }
}

void CACHE::llc_replacement_final_stats()
{
}
