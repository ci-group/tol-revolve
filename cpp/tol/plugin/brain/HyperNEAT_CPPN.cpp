#include <ios>

#include "HyperNEAT_CPPN.h"

#include "revolve/gazebo/motors/Motor.h"
#include "revolve/gazebo/sensors/Sensor.h"

#include "../Actuator.h"
#include "BodyParser.h"
#include "Helper.h"
#include "brain/Conversion.h"

namespace rb = revolve::brain;
namespace rg = revolve::gazebo;

namespace tol
{
  HyperNEAT_CPG::HyperNEAT_CPG(
          const std::string &_name,
          sdf::ElementPtr _brain,
          tol::EvaluatorPtr _evaluator,
          const std::vector< rg::MotorPtr > &_actuators,
          const std::vector< rg::SensorPtr > &_sensors
  )
          : rb::ConverterSplitBrain< rb::CPPNConfigPtr,
                                     CPPNEAT::GeneticEncodingPtr >
          (&rb::convertGEtoNN,
           &rb::convertNNtoGE,
           _name)
  {
    auto logFile = _name + ".log";
    std::ifstream f1(logFile.c_str());
    if (f1.good()) {
      std::cout << "Experiment was already done" << std::endl;
      std::exit(0);
    }

    // Initialise controller
    std::string name(_name.substr(0, _name.find('-')) + ".yaml");
    BodyParser body(name);

    std::tie(rb::InputMap, rb::OutputMap) = body.InputOutputMap(
            _actuators,
            _sensors);

    rb::RafCpgNetwork = rb::convertForController(body.CoupledCpgNetwork());
    rb::neuron_coordinates = body.IdToCoordinatesMap();

    // Initialise controller
    controller_ = rb::RafCPGControllerPtr(new rb::RafCPGController(
            _name,
            rb::RafCpgNetwork,
            Helper::createWrapper(_actuators),
            Helper::createWrapper(_sensors))
    );

    // Initialise learner
    auto learnConf = parseLearningSDF(_brain);
    rb::SetBrainSpec(true);
    learnConf.start_from = body.CppnNetwork();
    CPPNEAT::MutatorPtr mutator(new CPPNEAT::Mutator(
            rb::brain_spec,
            0.8,
            learnConf.start_from->InnovationsRange().second,
            100,
            std::vector< CPPNEAT::Neuron::Ntype >())
    );

    std::string parent1 = "none";
    std::string parent1brain = "none";
    std::string parent2 = "none";
    std::string parent2brain = "none";

    if ("none" not_eq learnConf.parent1)
    {
      parent1 = (_name.substr(0, _name.find('_')) + "_"
                          + learnConf.parent1
                          + _name.substr(_name.find('-'))
                          + ".innovations");
      parent1brain = (_name.substr(0, _name.find('_')) + "_"
                               + learnConf.parent1
                               + _name.substr(_name.find('-'))
                               + ".best");
    }

    if ("none" not_eq learnConf.parent1)
    {
      parent2 = (_name.substr(0, _name.find('_')) + "_"
                          + learnConf.parent2
                          + _name.substr(_name.find('-'))
                          + ".innovations");
      parent2brain = (_name.substr(0, _name.find('_')) + "_"
                               + learnConf.parent2
                               + _name.substr(_name.find('-'))
                               + ".best");
    }
    // initialise learner
    this->learner_ =
            boost::shared_ptr< CPPNEAT::NEATLearner >(new CPPNEAT::NEATLearner(
                    mutator,
                    _name + ".innovations",
                    parent1, // "none",  // parent1,
                    parent2, // "none",  // parent2,
                    learnConf)
            );

    std::cout << "After learner" << std::endl;
    auto brainsFromInit =
            boost::dynamic_pointer_cast< CPPNEAT::NEATLearner >(learner_)->InitCppns();
    std::vector< CPPNEAT::GeneticEncodingPtr > brainsFromFirst;
    if ("" == learnConf.parent1 or "none" == learnConf.parent1)
    {
      learnConf.num_first = 0;
    }
    else
    {
      brainsFromFirst = boost::dynamic_pointer_cast< CPPNEAT::NEATLearner >(
              learner_)->LoadCppns(parent1brain, -1);
    }

    std::vector< CPPNEAT::GeneticEncodingPtr > brainsFromSecond;
    if ("" == learnConf.parent2 or "none" == learnConf.parent2)
    {
      learnConf.num_second = 0;
    }
    else
    {
      brainsFromSecond = boost::dynamic_pointer_cast< CPPNEAT::NEATLearner >(
              learner_)->LoadSecondCppns(parent2brain, -1);
    }

    std::vector< CPPNEAT::GeneticEncodingPtr > init_brains;
    int cur_number = 0;
    int i = 0;
    while (cur_number < learnConf.num_first)
    {
      init_brains.push_back(brainsFromFirst[i]);
      i++;
      cur_number++;
    }
    i = 0;
    while (cur_number < learnConf.num_first + learnConf.num_second)
    {
      init_brains.push_back(brainsFromSecond[i]);
      i++;
      cur_number++;
    }
    i = 0;
    while (cur_number < learnConf.pop_size)
    {
      init_brains.push_back(brainsFromInit[i]);
      i++;
      cur_number++;
    }
    boost::dynamic_pointer_cast< CPPNEAT::NEATLearner >(learner_)->Initialise(
            init_brains);

    // initialise evaluator
    this->evaluator_ = _evaluator;

    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    std::cout << learnConf.num_first << " : " << learnConf.num_second << std::endl;
  }

  HyperNEAT_CPG::~HyperNEAT_CPG() = default;

  void HyperNEAT_CPG::update(
          const std::vector< rg::MotorPtr > &actuators,
          const std::vector< rg::SensorPtr > &sensors,
          double t,
          double step)
  {
    rb::ConverterSplitBrain< boost::shared_ptr< rb::CPPNConfig >,
                             CPPNEAT::GeneticEncodingPtr >::update(
            Helper::createWrapper(actuators),
            Helper::createWrapper(sensors),
            t,
            step
    );
  }

  CPPNEAT::NEATLearner::LearningConfiguration
  HyperNEAT_CPG::parseLearningSDF(sdf::ElementPtr _brain)
  {
    CPPNEAT::NEATLearner::LearningConfiguration config;

    // Read out brain configuration attributes
    config.asexual =
            _brain->HasAttribute("asexual") ?
            (_brain->GetAttribute("asexual")->GetAsString() == "true") :
            CPPNEAT::NEATLearner::ASEXUAL;
    config.pop_size =
            _brain->HasAttribute("pop_size") ?
            std::stoi(_brain->GetAttribute("pop_size")->GetAsString()) :
            CPPNEAT::NEATLearner::POP_SIZE;
    config.tournament_size =
            _brain->HasAttribute("tournament_size") ?
            std::stoi(_brain->GetAttribute("tournament_size")->GetAsString()) :
            CPPNEAT::NEATLearner::TOURNAMENT_SIZE;
    config.num_children =
            _brain->HasAttribute("num_children") ?
            std::stoi(_brain->GetAttribute("num_children")->GetAsString()) :
            CPPNEAT::NEATLearner::NUM_CHILDREN;
    config.weight_mutation_probability =
            _brain->HasAttribute("weight_mutation_probability") ?
            std::stod(_brain->GetAttribute("weight_mutation_probability")->GetAsString()) :
            CPPNEAT::NEATLearner::WEIGHT_MUTATION_PROBABILITY;
    config.weight_mutation_sigma =
            _brain->HasAttribute("weight_mutation_sigma") ?
            std::stod(_brain->GetAttribute("weight_mutation_sigma")->GetAsString()) :
            CPPNEAT::NEATLearner::WEIGHT_MUTATION_SIGMA;
    config.param_mutation_probability =
            _brain->HasAttribute("param_mutation_probability") ?
            std::stod(_brain->GetAttribute("param_mutation_probability")->GetAsString()) :
            CPPNEAT::NEATLearner::PARAM_MUTATION_PROBABILITY;
    config.param_mutation_sigma =
            _brain->HasAttribute("param_mutation_sigma") ?
            std::stod(_brain->GetAttribute("param_mutation_sigma")->GetAsString()) :
            CPPNEAT::NEATLearner::PARAM_MUTATION_SIGMA;
    config.structural_augmentation_probability =
            _brain->HasAttribute("structural_augmentation_probability") ?
            std::stod(_brain->GetAttribute("structural_augmentation_probability")->GetAsString()) :
            CPPNEAT::NEATLearner::STRUCTURAL_AUGMENTATION_PROBABILITY;
    config.structural_removal_probability =
            _brain->HasAttribute("structural_removal_probability") ?
            std::stod(_brain->GetAttribute("structural_removal_probability")->GetAsString()) :
            CPPNEAT::NEATLearner::STRUCTURAL_REMOVAL_PROBABILITY;
    config.max_generations =
            _brain->HasAttribute("max_generations") ?
            std::stoi(_brain->GetAttribute("max_generations")->GetAsString()) :
            CPPNEAT::NEATLearner::MAX_GENERATIONS;
    config.speciation_threshold =
            _brain->HasAttribute("speciation_threshold") ?
            std::stod(_brain->GetAttribute("speciation_threshold")->GetAsString()) :
            CPPNEAT::NEATLearner::SPECIATION_TRESHOLD;
    config.repeat_evaluations =
            _brain->HasAttribute("repeat_evaluations") ?
            std::stoi(_brain->GetAttribute("repeat_evaluations")->GetAsString()) :
            CPPNEAT::NEATLearner::REPEAT_EVALUATIONS;
    config.initStructMutations =
            _brain->HasAttribute("initial_structural_mutations") ?
            std::stoi(_brain->GetAttribute("initial_structural_mutations")->GetAsString()) :
            CPPNEAT::NEATLearner::INITIAL_STRUCTURAL_MUTATIONS;
    config.interspecies_mate_probability =
            _brain->HasAttribute("interspecies_mate_probability") ?
            std::stod(_brain->GetAttribute("interspecies_mate_probability")->GetAsString()) :
            CPPNEAT::NEATLearner::INTERSPECIES_MATE_PROBABILITY;
    //initialise starting population
    config.num_first  =
            _brain->HasAttribute("num_first") ?
            std::stoi(_brain->GetAttribute("num_first")->GetAsString()) : 0;
    config.num_second =
            _brain->HasAttribute("num_second") ?
            std::stoi(_brain->GetAttribute("num_second")->GetAsString()) : 0;
    //initialise starting population
    config.parent1 = _brain->HasAttribute("parent1") ?
                      _brain->GetAttribute("parent1")->GetAsString() : "none";
    config.parent2 = _brain->HasAttribute("parent2") ?
                     _brain->GetAttribute("parent2")->GetAsString() : "none";

    return config;
  }

} /* namespace tol */
