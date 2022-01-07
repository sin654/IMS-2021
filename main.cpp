/**
 * @file main.cpp
 * @author Adam Gajda (xgajda07@stud.fit.vutbr.cz), Jan Doležel (xdolez81@stud.fit.vutbr.cz)
 * @brief Simulation of bakery and it's supply chain with SIMLIB
 * 
 * @date 2021-12-12
 */

/**
 * @brief LIBRARIES
 * 
 */
#include "simlib.h"
#include <getopt.h>
#include "iostream"

/**
 * @brief MACROS
 * 
 */
#define SECOND * c_SECOND
#define MINUTE * c_MINUTE
#define HOUR * c_HOUR
#define DAY * c_DAY
#define MONTH * c_MONTH
#define YEAR * c_YEAR


/**
 * @brief GLOBALS
 * 
 */
long long wheat_plants = 0;
long long wheat_grains = 0;
long long plant_matter = 0;
long long flour_to_deliver = 0; 
long long flourKg = 0; 
long long piecesOfDoughForRounding = 0;
long long piecesOfDoughForBaking = 0;
long long breadPiecesForSale = 0;
long long bread_counter = 0;
long long bread_rustic = 0;
long long bread_roll = 0;
long long bread_french = 0;

const unsigned long long c_SECOND = 1;
const unsigned long long c_MINUTE = c_SECOND * 60;
const unsigned long long c_HOUR = c_MINUTE * 60;
const unsigned long long c_DAY = c_HOUR * 24;
const unsigned long long c_MONTH = c_DAY * 30;
const unsigned long long c_YEAR = c_MONTH * 12;

const unsigned long long GEN_CUSTOMER = 2 MINUTE; // 2 min in seconds
const unsigned long long GEN_WHEAT = 8 MONTH; // eight 30day months in seconds
const unsigned long long GEN_WHEAT_WAIT = 4 MONTH; // four 30day months in seconds
unsigned long long NM_OF_ACRES = 10;
unsigned long long NM_OF_THRESHERS = 1;
unsigned long long NM_OF_MILLS = 1;
unsigned long long NM_OF_SHOPKEEPERS = 1;
unsigned long long NM_OF_BAKERS = 5;
const unsigned long long NM_OF_INGREDIENT_MIXERS = 1;
const unsigned long long NM_OF_DOUGH_MIXERS = 1;
const unsigned long long NM_OF_DOUGH_DIVIDER = 1;
const unsigned long long NM_OF_ROUNDING_TABLES = 2;
const unsigned long long NM_OF_OVENS = 2;

// simulation time
unsigned long long simulation_time = 3 YEAR;
// default output file
std::string output_file = "bread.dat";

Store Baker("Bakers", NM_OF_BAKERS);
Store IngredientMixer("Ingredient Mixer", NM_OF_INGREDIENT_MIXERS);
Store DoughMixer("Dough Mixer", NM_OF_DOUGH_MIXERS);
Store DoughDivider("Dough Divider", NM_OF_DOUGH_DIVIDER);
Store RoundingTable("Rounding Tables", NM_OF_ROUNDING_TABLES);
Store Oven("Ovens", NM_OF_OVENS);
Store Wheat_thresher("Wheat thresher", NM_OF_THRESHERS);
Store Wheat_mill("Wheat mill", NM_OF_MILLS);
Store Shopkeeper("Shopkeeper", NM_OF_SHOPKEEPERS);

Facility Farmer("Farmer");
Facility Car("Car");

Queue passivatedBulkToPieces;
//Queue passivatedPiecesToRoundedPieces; //not uset anymore (just a reminder)
Queue passivatedRoundedPiecesToBread;

// statistics
//Histogram celkBulkToPieces("Time in system: Bulk to pieces", 0, 1000, 50);
//Histogram celkPiecesToRoundedPieces("Time in system: Pieces to rounded pieces", 0, 1000, 50);
//Histogram celkRoundedPiecesToBread("Time in system: Rounded pieces to bread", 0, 1000, 50);


class WheatProcessing : public Process{
    void Behavior(){
        while(wheat_plants >= 233)
        {
            wheat_plants -= 233;
            Enter(Wheat_thresher, 1);
            Wait(1 DAY);
            plant_matter += 133;
            wheat_grains += 100;
            Leave(Wheat_thresher);
        }
    }
};

class Plant_matter : public Process{
    void Behavior(){
        do{
            Seize(Farmer);
            Wait(1 DAY);
            Release(Farmer);
        }while(plant_matter >= 5);
    }
};

class Wheat_grains : public Process{
    void Behavior(){
        do{
            Enter(Wheat_mill);
            Wait(1 HOUR);
            
            flour_to_deliver += 50;
            Leave(Wheat_mill);
        }while(wheat_grains >= 50);
    }
};

class Flour_transport : public Process{
    void Behavior(){
        do{
            Seize(Car);
            Wait(2 HOUR);
            flourKg += 1000;
            Release(Car);
        }while(flour_to_deliver >= 1000);
    }
};

class Bakery : public Process{
    void Behavior(){
        if(bread_counter < 1) // if no bread around customer will immediately leave
            return;

        Enter(Shopkeeper);

        double p = Uniform(0, 100);
        if(p <= 40) //bread rustic
        {
            if(bread_rustic < 1) //after customer finds out that no rustic bread is present, customer leaves
            {
                Leave(Shopkeeper);
                return;
            }

            bread_rustic--;
        }
        else if(p > 40 && p <= 85) //bread roll
        {
            if(bread_roll < 1) //after customer finds out that no bread roll is present, customer leaves
            {
                Leave(Shopkeeper);
                return;
            }

            bread_roll--;
        }
        else //bread french
        {
            if(bread_french < 1) //after customer finds out that no french bread is present, customer leaves
            {
                Leave(Shopkeeper);
                return;
            }

            bread_french--;
        }

        bread_counter--;
        Wait(Uniform(1 MINUTE, 3 MINUTE));

        Leave(Shopkeeper);
    }
};

class BulkToPieces : public Process{
    void Behavior(){
        //double prichod = Time;  // for statistics
        
        while(1)
        {
            if(!Baker.Full() && !IngredientMixer.Full())  // there is free Baker and IngredientMixer
            {
                break;
            }
            else
            {
                passivatedBulkToPieces.Insert(this);
                Passivate();
            }
        }
        
        Enter(IngredientMixer, 1);
        Enter(Baker, 1);
        Wait(Uniform(4 MINUTE, 5 MINUTE)); // 4-5min Put ingredients into mixer
        Leave(Baker, 1);
        Wait(9 MINUTE); // 9min Mixing ingredients
        Enter(Baker, 1);
        Wait(Uniform(1 MINUTE, 2 MINUTE)); // 1-2min Put dough out of mixer
        Leave(IngredientMixer, 1);
        Wait(Uniform(2 MINUTE, 3 MINUTE)); // 2-3min Prepare dough for fermentation
        Leave(Baker, 1);
        Wait(1 HOUR); // 1hour Dough fermentation
        Enter(Baker, 1);
        Wait(Uniform(1 MINUTE, 2 MINUTE)); // 1-2min Dough transfer
        Enter(DoughMixer, 1);
        Wait(Uniform(1 MINUTE, 2 MINUTE)); // 1-2min Mixer filling
        Leave(Baker, 1);
        Wait(5 MINUTE); // 5min Dough mixing
        Enter(Baker, 1);
        Wait(Uniform(1 MINUTE, 2 MINUTE)); //1-2min Dough out of mixer
        Leave(DoughMixer, 1);
        Wait(Uniform(2 MINUTE, 3 MINUTE)); // 2-3min Dough transfer
        Enter(DoughDivider, 1);
        Wait(Uniform(1 MINUTE, 2 MINUTE)); // 1-2min Dough into divider
        Leave(Baker, 1);
        for(int i=0; i<60; i++)
        {
            Wait(10 SECOND); // 10s to make in piece of dough for rounding in divider
            piecesOfDoughForRounding++;
        }
        Leave(DoughDivider, 1);
        //celkBulkToPieces(Time - prichod); // statistics
    } // end Behavior

};  // end BulkToPieces

class PiecesToRoundedPieces : public Process{
    void Behavior(){
        if(!Baker.Full() && !RoundingTable.Full())
        {
            Enter(Baker, 1);
            Enter(RoundingTable, 1);
            // check if there is still another place at rounding table and free baker (if so then start another of this processes)
            if(!Baker.Full() && !RoundingTable.Full())
                (new PiecesToRoundedPieces)->Activate();
            while(piecesOfDoughForRounding > 0)
            {
                piecesOfDoughForRounding--; // take on piece of dough
                Wait(Uniform(30 SECOND, 1 MINUTE)); // 30-60s Dough rounding
                piecesOfDoughForBaking++; // add rounded dough to baking pieces
            }
            Leave(RoundingTable, 1);
            Leave(Baker, 1);
        }
    } // end Behavior

};  // end PiecesToRoundedPieces

class RoundedPiecesToBread : public Process{
    void Behavior(){
        //double prichod = Time;  // statistics
        
        while(1)
        {
            if(!Baker.Full() && !Oven.Full())  // there is free Baker and RoundingTable
            {
                break;
            }
            else
            {
                passivatedRoundedPiecesToBread.Insert(this);
                Passivate();
            }
        }
        
        Enter(Oven, 1);
        Enter(Baker, 1);
        Wait(Uniform(1 MINUTE, 2 MINUTE)); // 1-2min Fill oven
        Leave(Baker, 1);
        Wait(20 MINUTE); // 20min Baking
        Priority = 10;  // add priotity to this precess in queues
        Enter(Baker, 1);
        Wait(30 SECOND); // 30s Bread out of oven
        Leave(Oven, 1);
        Wait(Uniform(3 MINUTE, 4 MINUTE)); // 3-4min Transfer to store
        Leave(Baker, 1);
        breadPiecesForSale += 20; // add 20 breads to store, where they can be sold

        double p = Uniform(0, 100);
        if(p <= 34)
        {
            bread_rustic += 20;
        }
        else if(p > 34 && p <= 67)
        {
            bread_roll += 20;
        }
        else
        {
            bread_french += 20;
        }
        bread_counter += 20;

        //celkRoundedPiecesToBread(Time - prichod);   // statistics
    } // end Behavior

};  // end RoundedPiecesToBread

class BulkToPiecesGener : public Event{
    void Behavior(){
        while(flourKg > 18)
        {
            flourKg -= 18; //take 60kg of wheat flour
            (new BulkToPieces)->Activate();
        }
        Activate(Time + 1); // check every hour if we didnt't get more supply of wheat flour
    }   // end Behavior
};  // end BulkToPiecesGener

class PiecesToRoundedPiecesGener : public Event{
    void Behavior(){
        if(piecesOfDoughForRounding > 0)
        {
            (new PiecesToRoundedPieces)->Activate();
        }
        Activate(Time + 1); // check every minute if there are some pieces of dough to round
    }   // end Behavior
};  // end PiecesToRoundedPiecesGener

class RoundedPiecesToBreadGener : public Event{
    void Behavior(){
        while(piecesOfDoughForBaking >= 20)    // 20 bread to fill one oven
        {
            piecesOfDoughForBaking -= 20; // take 20 rounded bread
            (new RoundedPiecesToBread)->Activate();
        }
        Activate(Time + 1);
    }   // end Behavior
};  // end RoundedPiecesToBreadGener

class StartProcesses : public Event{
    void Behavior(){
        if(Random() <= 0.5)
        {
            if(!passivatedBulkToPieces.Empty()) // there is something in the Queue
                (passivatedBulkToPieces.GetFirst())->Activate();
            if(!passivatedRoundedPiecesToBread.Empty())
                (passivatedRoundedPiecesToBread.GetFirst())->Activate();
        }
        else
        {
            if(!passivatedRoundedPiecesToBread.Empty())
                (passivatedRoundedPiecesToBread.GetFirst())->Activate();
            if(!passivatedBulkToPieces.Empty()) // there is something in the Queue
                (passivatedBulkToPieces.GetFirst())->Activate();
        }
        Activate(Time + 1);
    }   // end Behavior
};  // end RoundedPiecesToBreadGener

class Generator_wheat_plants : public Event
{
    void Behavior()
    {
        wheat_plants += 3045;
        (new WheatProcessing)->Activate();
        Activate(Time + GEN_WHEAT + GEN_WHEAT_WAIT);
    }
};

class Generator_plant_matter : public Event{
    void Behavior(){
        if(plant_matter >= 5)
        {
            plant_matter -= 5;
            (new Plant_matter)->Activate();
        }
        Activate(Time + 1); // check every minute if there are at least 5 kilograms of plant matter
    }   // end Behavior
};  // end Generator_plant_matter

class Generator_wheat_grains : public Event{
    void Behavior(){
        if(wheat_grains >= 50)
        {
            wheat_grains -= 50;
            (new Wheat_grains)->Activate();
        }
        Activate(Time + 1); // check every minute if there are at least 50 kilograms of grain
    }   // end Behavior
};  // end Generator_wheat_grains

class Generator_flour_transport : public Event{
    void Behavior(){
        if(flour_to_deliver >= 1000)
        {
            flour_to_deliver -= 1000;
            (new Flour_transport)->Activate();
        }
        Activate(Time + 1); // check every minute if there are at least 50 kilograms of grain
    }   // end Behavior
};  // end Generator_flour_transport

class Generator_customer : public Event
{
    void Behavior()
    {
        (new Bakery)->Activate();
        Activate(Time + Exponential(GEN_CUSTOMER));
    }
};

int parse_args(int argc, char *argv[])
{
    char *endptr;
    long long tmp_ll = 0;
    
    int opt = 0;
    while((opt = getopt(argc, argv, "a:t:m:s:b:o:d:")) != -1)
    {
        switch (opt)
        {
        case 'a': //NM_OF_ACRES
            tmp_ll = strtoll(optarg, &endptr, 10);
            NM_OF_ACRES = (unsigned long long)tmp_ll;
            break;
        case 't': //NM_OF_THRESHERS
            tmp_ll = strtoll(optarg, &endptr, 10);
            NM_OF_THRESHERS = (unsigned long long)tmp_ll;
            break;
        case 'm': //NM_OF_MILLS
            tmp_ll = strtoll(optarg, &endptr, 10);
            NM_OF_MILLS = (unsigned long long)tmp_ll;
            break;
        case 's': //NM_OF_SHOPKEEPERS
            tmp_ll = strtoll(optarg, &endptr, 10);
            NM_OF_SHOPKEEPERS = (unsigned long long)tmp_ll;
            break;
        case 'b': //NM_OF_BAKERS
            tmp_ll = strtoll(optarg, &endptr, 10);
            NM_OF_BAKERS = (unsigned long long)tmp_ll;
            break;
        case 'o': //output_file
            output_file = optarg;
            break;
        case 'd': //simulation_time
            tmp_ll = strtoll(optarg, &endptr, 10);
            simulation_time = (unsigned long long)tmp_ll * 1 MONTH; 
            break;
        
        default:
            printf("Usage: bread [-a N] [-t N] [-m N] [-s N] [-b N] [-o STRING] [-d N]\n");
            fprintf(stderr, "\nError: Unknown specified parameter.\n\n");
            return -1;
            break;
        }

        if(opt != 'o')
        {
            if(*endptr != '\0' || tmp_ll < 0)
            {
                printf("Usage: bread [-a N] [-t N] [-m N] [-s N] [-b N] [-o STRING] [-d N]\n");
                fprintf(stderr, "\nError: Parameter %c only accepts valid positive integers.\n\n", opt);
                return -1;
            }
        }
    }

    Wheat_thresher.SetCapacity(NM_OF_THRESHERS);
    Wheat_mill.SetCapacity(NM_OF_MILLS);
    Shopkeeper.SetCapacity(NM_OF_SHOPKEEPERS);
    Baker.SetCapacity(NM_OF_BAKERS);

    return 0;
}

/**
 * @brief Main body of simulation
 * 
 * @param argc num of arguments
 * @param argv argument values
 * @return int return code
 */
int main(int argc, char *argv[])
{   
    if(parse_args(argc, argv) == -1)
        return -1;
        

    SetOutput(output_file.c_str());
    Init(0, simulation_time);
    
    for (size_t i = 0; i < NM_OF_ACRES; i++)
        (new Generator_wheat_plants)->Activate();
    (new Generator_plant_matter)->Activate();
    (new Generator_wheat_grains)->Activate();
    (new Generator_flour_transport)->Activate();
    
    (new BulkToPiecesGener)->Activate();
    (new PiecesToRoundedPiecesGener)->Activate();
    (new RoundedPiecesToBreadGener)->Activate();

    (new Generator_customer)->Activate();

    (new StartProcesses)->Activate();
    
    Run();

    
    // tisk statistik
    Wheat_thresher.Output();
    Wheat_mill.Output();
    Baker.Output();
    IngredientMixer.Output();
    DoughMixer.Output();
    DoughDivider.Output();
    RoundingTable.Output();
    Oven.Output();
    Shopkeeper.Output();

    //histograms
    //celkBulkToPieces.Output();
    //celkPiecesToRoundedPieces.Output();
    //celkRoundedPiecesToBread.Output();

    std::cout << "Wheat plant left: " << wheat_plants << std::endl;
    std::cout << "Plant matter left: " << plant_matter << std::endl;
    std::cout << "Wheat grain left: " << wheat_grains << std::endl;
    std::cout << "Wheat flour left to deliver: " << flour_to_deliver << std::endl;
    std::cout << "Wheat flour left delivered: " << flourKg << std::endl;
    std::cout << "Dough left to be rounded: " << piecesOfDoughForRounding << std::endl;
    std::cout << "Dough left to be baked: " << piecesOfDoughForBaking << std::endl;
    std::cout << "Bread total created: " << breadPiecesForSale << std::endl;
    std::cout << "Unsold bread: " << bread_counter << std::endl;
    std::cout << "Simulation time [s]: " << simulation_time << std::endl;
    
    return 0;
}