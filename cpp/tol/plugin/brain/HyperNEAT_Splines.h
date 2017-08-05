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
* Author: TODO <Add proper author>
*
*/

#ifndef TOL_PLUGIN_HYPERNEAT_SPLINE_H_
#define TOL_PLUGIN_HYPERNEAT_SPLINE_H_

#include <string>
#include <vector>

#include <gazebo/gazebo.hh>

#include <revolve/msgs/neural_net.pb.h>
#include "revolve/gazebo/brain/Brain.h"

#include "Evaluator.h"
#include "brain/ConverterSplitBrain.h"
#include "brain/controller/PolicyController.h"
#include "brain/learner/NEATLearner.h"
#include "brain/learner/RLPowerLearner.h"

namespace tol
{
  class HyperNEAT_Splines
          : public revolve::gazebo::Brain
            , private revolve::brain::ConverterSplitBrain
                  <revolve::brain::PolicyPtr,
                   cppneat::GeneticEncodingPtr>
  {
    public:

    /// \brief Constructor for a neural network including neurons that are of a
    /// different type than the usual ones.
    /// \param modelName: name of the model
    /// \param evaluator: pointer to the evaluator that is used
    /// \param node: the sdf file containing the necessary information to build
    /// the network
    /// \param actuators: vector list of robot's actuators
    /// \param sensors: vector list of robot's sensors
    /// \return pointer to the neural network
    HyperNEAT_Splines(std::string modelName,
                      sdf::ElementPtr brain,
                      tol::EvaluatorPtr evaluator,
                      const std::vector<revolve::gazebo::MotorPtr> &actuators,
                      const std::vector<revolve::gazebo::SensorPtr> &sensors);

    /// \brief Destructor
    virtual ~HyperNEAT_Splines();

    using revolve::brain::ConverterSplitBrain
            <revolve::brain::PolicyPtr,
             cppneat::GeneticEncodingPtr>::update;

    /// \brief Update sensors reading, actuators position, and `brain` state
    /// \param[inout] actuators List of actuators
    /// \param[inout] sensors List of sensors
    /// \param[in] t Time value
    /// \param[in] step Time step
    virtual void update(const std::vector<revolve::gazebo::MotorPtr> &actuators,
                        const std::vector<revolve::gazebo::SensorPtr> &sensors,
                        double t,
                        double step);

    /// \brief
    static revolve::brain::RLPowerLearner::Config
    parseSDF(sdf::ElementPtr brain);

    /// \brief
    static cppneat::NEATLearner::LearningConfiguration parseLearningSDF(
            sdf::ElementPtr brain);
  };
} /* namespace tol */

#endif  //  TOL_PLUGIN_HYPERNEAT_SPLINE_H_


