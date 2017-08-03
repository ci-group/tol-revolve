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

#include "test_YAMLBodyParser.h"

#define BOOST_TEST_MODULE TestYAMLBodyParser test
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>

#include "../brain/YamlBodyParser.h"

void testRobot(const std::string &yaml_source,
               const std::vector <std::vector<bool>> &connections_target,
               const std::vector <std::vector<float>> &coordinates_target)
{
    tol::YamlBodyParser* parser = new tol::YamlBodyParser();
    parser->parseCode(yaml_source);

    // Check connections
    std::vector <std::vector<bool>> connections = parser->connections();
    BOOST_CHECK_EQUAL(connections.size(), connections_target.size());
    for (size_t i = 0; i< connections.size(); i++) {
//        std::cout << "connections[" << i << ']' << std::endl;
        BOOST_CHECK_EQUAL_COLLECTIONS(connections[i].begin(), connections[i].end(),
                                      connections_target[i].begin(), connections_target[i].end());
    }

    // Check coordinates
    std::vector <std::vector<float>> coordinates = parser->coordinates();
    BOOST_CHECK_EQUAL(coordinates.size(), coordinates_target.size());
    for (size_t i = 0; i< coordinates.size(); i++) {
//        std::cout << "coordinates[" << i << ']' << std::endl;
        for (size_t j = 0; j< coordinates[i].size(); j++) {
            BOOST_CHECK_CLOSE(coordinates[i][j], coordinates_target[i][j], 1);
        }
    }
}

BOOST_AUTO_TEST_CASE(yaml_body_parser_just_runs)
{
    tol::YamlBodyParser* parser = new tol::YamlBodyParser();
    parser->parseFile("../res/robots/spider9.yaml");
    std::vector <std::vector<bool>> connections = parser->connections();
    std::vector <std::vector<float>> coordinates = parser->coordinates();
}

BOOST_AUTO_TEST_CASE(yaml_body_parser_spider)
{
    testRobot(tol::Spider9_yaml_source,
              {
              #define X true,
              #define _ false,
                      { _ X X _ X _ X _ } ,
                      { X _ _ _ _ _ _ _ } ,
                      { X _ _ X X _ X _ } ,
                      { _ _ X _ _ _ _ _ } ,
                      { X _ X _ _ X X _ } ,
                      { _ _ _ _ X _ _ _ } ,
                      { X _ X _ X _ _ X } ,
                      { _ _ _ _ _ _ X _ } ,
              #undef X
              #undef _
              },
              {
                      // Leg00Joint Leg01Joint
                      {-0.25f,    0},
                      {-0.75f,    0},
                      // Leg10Joint Leg11Joint
                      {0.25f,     0},
                      {0.75f,     0},
                      // Leg20Joint Leg21Joint
                      {0,    -0.25f},
                      {0,    -0.75f},
                      // Leg30Joint Leg31Joint
                      {0,     0.25f},
                      {0,     0.75f},
              }
    );

    testRobot(tol::Spider13_yaml_source,
              {
              #define X true,
              #define _ false,
                      {_ X _ X _ _ X _ _ X _ _ } ,//
                      {X _ X _ _ _ _ _ _ _ _ _ } ,
                      {_ X _ _ _ _ _ _ _ _ _ _ } ,
                      {X _ _ _ X _ X _ _ X _ _ } ,//
                      {_ _ _ X _ X _ _ _ _ _ _ } ,
                      {_ _ _ _ X _ _ _ _ _ _ _ } ,
                      {X _ _ X _ _ _ X _ X _ _ } ,//
                      {_ _ _ _ _ _ X _ X _ _ _ } ,
                      {_ _ _ _ _ _ _ X _ _ _ _ } ,
                      {X _ _ X _ _ X _ _ _ X _ } ,//
                      {_ _ _ _ _ _ _ _ _ X _ X } ,
                      {_ _ _ _ _ _ _ _ _ _ X _ } ,
              #undef X
              #undef _
              },
              {
                      // Leg00Joint Leg01Joint Leg02Joint
                      {-.1666666f, 0},
                      {-.5f,      0},
                      {-0.833333f, 0},
                      // Leg10Joint Leg11Joint Leg12Joint
                      { .1666666f, 0},
                      { .5f, 0},
                      { 0.833333f, 0},
                      // Leg20Joint Leg21Joint Leg22Joint
                      {0, -.1666666f},
                      {0, -.5f},
                      {0, -.833333f},
                      // Leg30Joint Leg31Joint Leg32Joint
                      {0,  .1666666f},
                      {0,  .5f},
                      {0,  .833333f},
              }
    );

    testRobot(tol::Spider17_yaml_source,
              {
              #define X true,
              #define _ false,
                      { _ X _ _ X _ _ _ X _ _ _ X _ _ _ } , //
                      { X _ X _ _ _ _ _ _ _ _ _ _ _ _ _ } ,
                      { _ X _ X _ _ _ _ _ _ _ _ _ _ _ _ } ,
                      { _ _ X _ _ _ _ _ _ _ _ _ _ _ _ _ } ,
                      { X _ _ _ _ X _ _ X _ _ _ X _ _ _ } , //
                      { _ _ _ _ X _ X _ _ _ _ _ _ _ _ _ } ,
                      { _ _ _ _ _ X _ X _ _ _ _ _ _ _ _ } ,
                      { _ _ _ _ _ _ X _ _ _ _ _ _ _ _ _ } ,
                      { X _ _ _ X _ _ _ _ X _ _ X _ _ _ } , //
                      { _ _ _ _ _ _ _ _ X _ X _ _ _ _ _ } ,
                      { _ _ _ _ _ _ _ _ _ X _ X _ _ _ _ } ,
                      { _ _ _ _ _ _ _ _ _ _ X _ _ _ _ _ } ,
                      { X _ _ _ X _ _ _ X _ _ _ _ X _ _ } , //
                      { _ _ _ _ _ _ _ _ _ _ _ _ X _ X _ } ,
                      { _ _ _ _ _ _ _ _ _ _ _ _ _ X _ X } ,
                      { _ _ _ _ _ _ _ _ _ _ _ _ _ _ X _ } ,
              #undef X
              #undef _
              },
              {
                      // Leg00Joint Leg01Joint Leg02Joint
                      {-.125f, 0},
                      {-.375f, 0},
                      {-.625f, 0},
                      {-.875f, 0},
                      // Leg10Joint Leg11Joint Leg12Joint
                      { .125f, 0},
                      { .375f, 0},
                      { .625f, 0},
                      { .875f, 0},
                      // Leg20Joint Leg21Joint Leg22Joint
                      {0, -.125f},
                      {0, -.375f},
                      {0, -.625f},
                      {0, -.875f},
                      // Leg30Joint Leg31Joint Leg32Joint
                      {0, .125f},
                      {0, .375f},
                      {0, .625f},
                      {0, .875f},
              }
    );
}
