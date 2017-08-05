/*
* Copyright (C) 2017 Vrije Universiteit Amsterdam
*
* Licensed under the Apache License, Version 2.0 (the "License");
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Description: TODO: <Add brief description about file purpose>
* Author: Milan Jelisavcic
* Date: March 28, 2016
*
*/

#include <string>
#include <vector>

#include "RLPower_Splines.h"

#include "revolve/gazebo/motors/Motor.h"
#include "revolve/gazebo/sensors/Sensor.h"

#include "BodyParser.h"
#include "Helper.h"
#include "brain/Conversion.h"
#include "brain/controller/ExtCPPNWeights.h"

namespace tol
{
  RLPower_Splines::RLPower_Splines(
          std::string model_name,
          sdf::ElementPtr brain,
          EvaluatorPtr evaluator,
          std::vector<revolve::gazebo::MotorPtr> &actuators,
          std::vector<revolve::gazebo::SensorPtr> &sensors)
          : revolve::brain::ConverterSplitBrain<revolve::brain::PolicyPtr,
                                                revolve::brain::PolicyPtr>
                    (&revolve::brain::convertPolicyToPolicy,
                     &revolve::brain::convertPolicyToPolicy,
                     model_name)
  {
    revolve::brain::RLPowerLearner::Config config = parseSDF(brain);
    // initialise controller
    unsigned int n_actuators = 0;
    for (auto it : actuators)
    {
      n_actuators += it->outputs();
    }
    controller_ = boost::shared_ptr<revolve::brain::PolicyController>(
            new revolve::brain::PolicyController(
                    n_actuators,
                    config.interpolation_spline_size));

    // initialise learner
    learner_ = boost::shared_ptr<revolve::brain::RLPowerLearner>
            (new revolve::brain::RLPowerLearner(model_name,
                                                config,
                                                n_actuators));
    evaluator_ = evaluator;
  }

  RLPower_Splines::~RLPower_Splines()
  {
  }

  void RLPower_Splines::update(
          const std::vector<revolve::gazebo::MotorPtr> &actuators,
                          const std::vector<revolve::gazebo::SensorPtr> &sensors,
                          double t,
                          double step)
  {
    revolve::brain::ConverterSplitBrain<revolve::brain::PolicyPtr,
                                        revolve::brain::PolicyPtr>::update(
            Helper::createWrapper(actuators),
            Helper::createWrapper(sensors),
            t,
            step);
  }

  revolve::brain::RLPowerLearner::Config
  RLPower_Splines::parseSDF(sdf::ElementPtr brain)
  {
    revolve::brain::RLPowerLearner::Config config;

    // Read out brain configuration attributes
    config.algorithm_type =
            brain->HasAttribute("type") ?
            brain->GetAttribute("type")->GetAsString() : "A";

    config.evaluation_rate =
            brain->HasAttribute("evaluation_rate") ?
            std::stod(brain->GetAttribute("evaluation_rate")->GetAsString()) :
                             revolve::brain::RLPowerLearner::EVALUATION_RATE;
    config.interpolation_spline_size =
            brain->HasAttribute("interpolation_spline_size") ?
            std::stoul(brain->GetAttribute("interpolation_spline_size")
                            ->GetAsString()) :
            revolve::brain::RLPowerLearner::INTERPOLATION_CACHE_SIZE;
    config.max_evaluations =
            brain->HasAttribute("max_evaluations") ?
            std::stoul(brain->GetAttribute("max_evaluations")->GetAsString()) :
            revolve::brain::RLPowerLearner::MAX_EVALUATIONS;
    config.max_ranked_policies =
            brain->HasAttribute("max_ranked_policies") ?
            std::stoul(brain->GetAttribute("max_ranked_policies")
                            ->GetAsString()) :
            revolve::brain::RLPowerLearner::MAX_RANKED_POLICIES;
    config.noise_sigma =
            brain->HasAttribute("init_sigma") ?
            std::stod(brain->GetAttribute("init_sigma")->GetAsString()) :
            revolve::brain::RLPowerLearner::SIGMA_START_VALUE;
    config.sigma_tau_correction =
            brain->HasAttribute("sigma_tau_correction") ?
            std::stod(brain->GetAttribute("sigma_tau_correction")
                           ->GetAsString()) :
            revolve::brain::RLPowerLearner::SIGMA_TAU_CORRECTION;
    config.source_y_size =
            brain->HasAttribute("init_spline_size") ?
            std::stoul(brain->GetAttribute("init_spline_size")->GetAsString()) :
            revolve::brain::RLPowerLearner::INITIAL_SPLINE_SIZE;
    config.update_step =
            brain->HasAttribute("update_step") ?
            std::stoul(brain->GetAttribute("update_step")->GetAsString()) :
            revolve::brain::RLPowerLearner::UPDATE_STEP;
    config.policy_load_path = "";

    return config;
  }
} /* namespace tol */
