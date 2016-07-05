/*
 Copyright 2001 The University of Texas at Austin

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "population.h"
#include "species/speciespopulation.h"
#include "multinnspecies/multinnspeciespopulation.h"
#include "util/util.h"

using namespace NEAT;
using namespace std;

Population *NEAT::debug_population = nullptr;

Population *Population::create(rng_t rng,
                               vector<unique_ptr<Genome>> &seeds) {
    Population *result;

    switch(env->population_type) {
    case PopulationType::SPECIES:
        result = new SpeciesPopulation(rng, seeds);
        break;
    case PopulationType::MULTI_NN_SPECIES:
        result = new MultiNNSpeciesPopulation(rng, seeds);
        break;
    default:
        panic();
    }

    debug_population = result;

    return result;
}
