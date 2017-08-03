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
* Author: Matteo De Carlo
*
*/

#include <gazebo/physics/Model.hh>

#include <revolve/gazebo/motors/Motor.h>
#include <revolve/gazebo/sensors/VirtualSensor.h>

#include "../FakeLightSensor.h"

#include "test_SUPGBrainPhototaxis.h"

// #define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MODULE SUPGBrainPhototaxis test
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <iostream>

class Motor : public revolve::gazebo::Motor {
public:
    Motor()
        : revolve::gazebo::Motor(
            nullptr, //::gazebo::physics::ModelPtr model
            "partId", //std::string partId
            "motorId", //std::string motorId
            1 // unsigned int outputs
        )
    {}

    virtual void update(double * output, double step) {}
};
typedef boost::shared_ptr< Motor > MotorPtr;

class Sensor : public revolve::gazebo::VirtualSensor {

};
typedef boost::shared_ptr< Sensor > SensorPtr;

class Evaluator : public tol::Evaluator {
public:
    Evaluator(double fitness) : _fitness(fitness) {}
    virtual void start() {}
    virtual double fitness() {
        return _fitness;
    }

    double _fitness;
};

BOOST_AUTO_TEST_CASE(create_instance)
{
    // GENERIC BRAIN STUFF
    boost::shared_ptr< Evaluator > evaluator_ = boost::make_shared<Evaluator>(1);

    std::vector< revolve::gazebo::MotorPtr > motors_
    ( {
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>()
    } );

    std::vector< revolve::gazebo::SensorPtr > sensors_
    ( {

    } );

    // SUPG BRAIN STUFF
    AsyncNeat::Init();
    std::vector< std::vector< float> > coordinates
    ( {
        // Leg00Joint
        { -.5, 0},
        // Leg01Joint
        { -1,  0},
        // Leg10Joint
        { +.5, 0},
        // Leg11Joint
        { +1,  0},
    } );

    // SPECIFIC SUPG BRAIN PHOTOTAXIS STUFF
    static const float fov = 150.0f;
    static const float sensor_offset = 0.05;

    revolve::brain::FakeLightSensor *sensor_left;
    revolve::brain::FakeLightSensor *sensor_right;

    std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> light_constructor_left;
    std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> light_constructor_right;

    auto light_constructor_common = [] (const std::vector<float> &coordinates,
                                        float sensor_offset)
                                    -> revolve::brain::FakeLightSensor*
    {
        if (coordinates.size() != 3) {
            std::cerr << "ERROR! COORDINATES TO THE LIGHT CONSTRUCTOR ARE THE WRONG SIZE!" << std::endl;
        }

        ignition::math::Vector3d light_position(
            coordinates[0],
            coordinates[1],
            coordinates[2]
        );

        return new tol::FakeLightSensor(
            "test_fake_light_sensor",
            fov,
            ignition::math::Pose3d(0,sensor_offset,0,0,0,0),
            light_position
        );
    };

    light_constructor_left = [&sensor_left, light_constructor_common] (std::vector<float> coordinates)
                             -> revolve::brain::FakeLightSensor *
    {
        delete sensor_left;
        sensor_left = light_constructor_common(coordinates, -sensor_offset);
        return sensor_left;
    };

    light_constructor_right = [&sensor_right, light_constructor_common] (std::vector<float> coordinates)
                              -> revolve::brain::FakeLightSensor *
    {
        delete sensor_right;
        sensor_right = light_constructor_common(coordinates, sensor_offset);
        return sensor_right;
    };

    testSUPGBrainPhototaxis testObject(
        evaluator_,
        light_constructor_left,
        light_constructor_right,
        0.5,
        coordinates,
        motors_,
        sensors_);

    AsyncNeat::CleanUp();
}


BOOST_AUTO_TEST_CASE(run_once)
{
    // GENERIC BRAIN STUFF
    boost::shared_ptr< Evaluator > evaluator_ = boost::make_shared<Evaluator>(1);

    std::vector< revolve::gazebo::MotorPtr > motors_
    ( {
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>()
    } );

    std::vector< revolve::gazebo::SensorPtr > sensors_
    ( {

    } );

    // SUPG BRAIN STUFF
    AsyncNeat::Init();
    std::vector< std::vector< float> > coordinates
    ( {
        // Leg00Joint
        { -.5, 0},
        // Leg01Joint
        { -1,  0},
        // Leg10Joint
        { +.5, 0},
        // Leg11Joint
        { +1,  0},
    } );

    // SPECIFIC SUPG BRAIN PHOTOTAXIS STUFF
    static const float fov = 150.0f;
    static const float sensor_offset = 0.05;

    revolve::brain::FakeLightSensor *sensor_left;
    revolve::brain::FakeLightSensor *sensor_right;

    std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> light_constructor_left;
    std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> light_constructor_right;

    auto light_constructor_common = [] (const std::vector<float> &coordinates,
                                        float sensor_offset)
                                    -> revolve::brain::FakeLightSensor*
    {
        if (coordinates.size() != 3) {
            std::cerr << "ERROR! COORDINATES TO THE LIGHT CONSTRUCTOR ARE THE WRONG SIZE!" << std::endl;
        }

        ignition::math::Vector3d light_position(
            coordinates[0],
            coordinates[1],
            coordinates[2]
        );

        return new tol::FakeLightSensor(
            "test_fake_light_sensor",
            fov,
            ignition::math::Pose3d(0,sensor_offset,0,0,0,0),
            light_position
        );
    };

    light_constructor_left = [&sensor_left, light_constructor_common] (std::vector<float> coordinates)
                             -> revolve::brain::FakeLightSensor *
    {
        delete sensor_left;
        sensor_left = light_constructor_common(coordinates, -sensor_offset);
        return sensor_left;
    };

    light_constructor_right = [&sensor_right, light_constructor_common] (std::vector<float> coordinates)
                              -> revolve::brain::FakeLightSensor *
    {
        delete sensor_right;
        sensor_right = light_constructor_common(coordinates, sensor_offset);
        return sensor_right;
    };

    testSUPGBrainPhototaxis testObject(
        evaluator_,
        light_constructor_left,
        light_constructor_right,
        0.5,
        coordinates,
        motors_,
        sensors_);

    testObject.update(motors_, sensors_, 1, 0.1);

    AsyncNeat::CleanUp();
}

BOOST_AUTO_TEST_CASE(run_multiple_times)
{
    // GENERIC BRAIN STUFF
    boost::shared_ptr< Evaluator > evaluator_ = boost::make_shared<Evaluator>(1);

    std::vector< revolve::gazebo::MotorPtr > motors_
    ( {
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>(),
        boost::make_shared<Motor>()
    } );

    std::vector< revolve::gazebo::SensorPtr > sensors_
    ( {

    } );

    // SUPG BRAIN STUFF
    AsyncNeat::Init();
    std::vector< std::vector< float> > coordinates
    ( {
        // Leg00Joint
        { -.5, 0},
        // Leg01Joint
        { -1,  0},
        // Leg10Joint
        { +.5, 0},
        // Leg11Joint
        { +1,  0},
    } );

    // SPECIFIC SUPG BRAIN PHOTOTAXIS STUFF
    static const float fov = 150.0f;
    static const float sensor_offset = 0.05;

    revolve::brain::FakeLightSensor *sensor_left;
    revolve::brain::FakeLightSensor *sensor_right;

    std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> light_constructor_left;
    std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> light_constructor_right;

    auto light_constructor_common = [] (const std::vector<float> &coordinates,
                                        float sensor_offset)
                                    -> revolve::brain::FakeLightSensor*
    {
        if (coordinates.size() != 3) {
            std::cerr << "ERROR! COORDINATES TO THE LIGHT CONSTRUCTOR ARE THE WRONG SIZE!" << std::endl;
        }

        ignition::math::Vector3d light_position(
            coordinates[0],
            coordinates[1],
            coordinates[2]
        );

        return new tol::FakeLightSensor(
            "test_fake_light_sensor",
            fov,
            ignition::math::Pose3d(0,sensor_offset,0,0,0,0),
            light_position
        );
    };

    light_constructor_left = [&sensor_left, light_constructor_common] (std::vector<float> coordinates)
                             -> revolve::brain::FakeLightSensor *
    {
        delete sensor_left;
        sensor_left = light_constructor_common(coordinates, -sensor_offset);
        return sensor_left;
    };

    light_constructor_right = [&sensor_right, light_constructor_common] (std::vector<float> coordinates)
                              -> revolve::brain::FakeLightSensor *
    {
        delete sensor_right;
        sensor_right = light_constructor_common(coordinates, sensor_offset);
        return sensor_right;
    };

    testSUPGBrainPhototaxis testObject(
        evaluator_,
        light_constructor_left,
        light_constructor_right,
        0.5,
        coordinates,
        motors_,
        sensors_);

    double step = 0.1;
    for (double time = 0; time < 100; time +=step)
        testObject.update(motors_, sensors_, time, step);

    AsyncNeat::CleanUp();
}

testSUPGBrainPhototaxis::testSUPGBrainPhototaxis(revolve::brain::EvaluatorPtr evaluator,
        std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_left,
        std::function<revolve::brain::FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_right,
        double light_radius_distance,
        const std::vector< std::vector< float > > &neuron_coordinates,
        const std::vector< revolve::gazebo::MotorPtr >& motors,
        const std::vector< revolve::gazebo::SensorPtr >& sensors)
    : tol::SUPGBrainPhototaxis(evaluator,
                               _light_constructor_left,
                               _light_constructor_right,
                               light_radius_distance,
                               neuron_coordinates,
                               motors,
                               sensors)
{
}

void testSUPGBrainPhototaxis::update(const std::vector< revolve::gazebo::MotorPtr >& motors,
                                     const std::vector< revolve::gazebo::SensorPtr >& sensors,
                                     double t, double step)
{
    tol::SUPGBrainPhototaxis::update(motors, sensors, t, step);
}
