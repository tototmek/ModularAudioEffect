#include "../src/blocks/evaluation_sequence.h"
#include "utils.h"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace test_utils;

TEST_CASE("Always pass", "[graphs]") {}

TEST_CASE("Simple graph", "[graphs]") {
    blocks::graph_t graph{{0, {1}}, {1, {2}}, {2, {}}};
    auto sequence = blocks::computeEvaluationSequence(graph);
    REQUIRE(compareVectors(sequence, {0, 1, 2}));
}

TEST_CASE("Simple long graph", "[graphs]") {
    blocks::graph_t graph;
    std::vector<uint> expectedResult;
    uint totalElements = 100;
    for (uint i = 0; i < totalElements; ++i) {
        graph.emplace(i, std::vector<uint>{i + 1});
        expectedResult.emplace_back(i);
    }
    graph.emplace(totalElements, std::vector<uint>{});
    expectedResult.emplace_back(totalElements);
    auto result = blocks::computeEvaluationSequence(graph);
    REQUIRE(compareVectors(expectedResult, result));
}

TEST_CASE("Two disconnected paths", "[graphs]") {
    blocks::graph_t graph{{0, {1}}, {1, {}}, {2, {3}}, {3, {}}};
    std::vector<uint> expectedResult1{2, 3, 0, 1};
    std::vector<uint> expectedResult2{2, 0, 3, 1};
    std::vector<uint> expectedResult3{2, 0, 1, 3};
    std::vector<uint> expectedResult4{0, 2, 3, 1};
    std::vector<uint> expectedResult5{0, 2, 1, 3};
    std::vector<uint> expectedResult6{0, 1, 2, 3};
    std::vector<uint> result = blocks::computeEvaluationSequence(graph);
    REQUIRE((compareVectors(expectedResult1, result) ||
             compareVectors(expectedResult2, result) ||
             compareVectors(expectedResult3, result) ||
             compareVectors(expectedResult4, result) ||
             compareVectors(expectedResult5, result) ||
             compareVectors(expectedResult6, result)));
}

TEST_CASE("Graph with two paths", "[graphs]") {
    blocks::graph_t graph{{0, {1, 3}}, {1, {2}}, {2, {3}}, {3, {}}};
    std::vector<uint> expectedResult{0, 1, 2, 3};
    std::vector<uint> result = blocks::computeEvaluationSequence(graph);
    REQUIRE(compareVectors(expectedResult, result));
}

TEST_CASE("Graph with two cycles", "[graphs]") {
    blocks::graph_t graph{{0, {1}}, {1, {2, 3}}, {2, {3}}, {3, {0}}};
    std::vector<uint> expectedResult1{0, 1, 2, 3};
    std::vector<uint> expectedResult2{1, 2, 3, 0};
    std::vector<uint> result = blocks::computeEvaluationSequence(graph);
    REQUIRE((compareVectors(expectedResult1, result) ||
             compareVectors(expectedResult2, result)));
}

TEST_CASE("Graph with two cycles, shuffled", "[graphs]") {
    blocks::graph_t graph{{3, {0}}, {1, {2, 3}}, {0, {1}}, {2, {3}}};
    std::vector<uint> expectedResult1{0, 1, 2, 3};
    std::vector<uint> expectedResult2{1, 2, 3, 0};
    std::vector<uint> result = blocks::computeEvaluationSequence(graph);
    REQUIRE((compareVectors(expectedResult1, result) ||
             compareVectors(expectedResult2, result)));
}

/*
TEST_CASE("Various graphs benchmark", "[graphs]") {
    BENCHMARK("8 nodes, 8 edges") {
        blocks::graph_t graph = {{0, {2, 5}},    {1, {}},     {2, {}}, {3, {1}},
                                 {4, {1, 1, 3}}, {5, {1, 0}}, {6, {}}, {7, {}}};
        return blocks::computeEvaluationSequence(graph);
    };
    BENCHMARK("10 nodes, 16 edges") {
        blocks::graph_t graph = {
            {0, {8, 5}},    {1, {9}},    {2, {2, 5}}, {3, {5}}, {4, {4, 3}},
            {5, {8, 8, 6}}, {6, {7, 5}}, {7, {9, 6}}, {8, {}},  {9, {6}}};
        return blocks::computeEvaluationSequence(graph);
    };
    BENCHMARK("20 nodes, 30 edges") {
        blocks::graph_t graph = {{0, {16}},
                                 {1, {1, 6}},
                                 {2, {9, 7, 4}},
                                 {3, {13}},
                                 {4, {10, 14, 6, 17}},
                                 {5, {}},
                                 {6, {8, 5}},
                                 {7, {3, 12, 1, 11}},
                                 {8, {2, 4, 0}},
                                 {9, {2, 17}},
                                 {10, {}},
                                 {11, {15}},
                                 {12, {}},
                                 {13, {4, 18}},
                                 {14, {2, 17}},
                                 {15, {3}},
                                 {16, {}},
                                 {17, {15}},
                                 {18, {}},
                                 {19, {9}}};
        return blocks::computeEvaluationSequence(graph);
    };
    BENCHMARK("60 nodes, 120 edges") {
        blocks::graph_t graph = {{0, {58, 54}},
                                 {1, {}},
                                 {2, {26, 51, 34, 49}},
                                 {3, {19, 45, 32}},
                                 {4, {50, 4}},
                                 {5, {25}},
                                 {6, {}},
                                 {7, {48, 23, 20}},
                                 {8, {13}},
                                 {9, {25, 7, 40, 23}},
                                 {10, {38, 40, 28}},
                                 {11, {23, 44, 54}},
                                 {12, {38, 13, 41, 52, 17, 38}},
                                 {13, {45, 59}},
                                 {14, {8, 30, 51}},
                                 {15, {}},
                                 {16, {59}},
                                 {17, {43}},
                                 {18, {}},
                                 {19, {11}},
                                 {20, {11}},
                                 {21, {10}},
                                 {22, {57, 10, 41, 59}},
                                 {23, {49, 12}},
                                 {24, {10, 29, 51, 36, 10}},
                                 {25, {58, 32, 18, 38, 6}},
                                 {26, {48, 36, 8}},
                                 {27, {9}},
                                 {28, {17, 16, 10, 37, 18, 37}},
                                 {29, {}},
                                 {30, {0}},
                                 {31, {31, 48}},
                                 {32, {}},
                                 {33, {31, 41}},
                                 {34, {54, 44}},
                                 {35, {}},
                                 {36, {50, 7, 19}},
                                 {37, {3, 43, 36, 49}},
                                 {38, {27, 53}},
                                 {39, {}},
                                 {40, {6}},
                                 {41, {52}},
                                 {42, {16, 53}},
                                 {43, {7, 20}},
                                 {44, {56, 1, 29, 49}},
                                 {45, {}},
                                 {46, {23}},
                                 {47, {13}},
                                 {48, {31, 44}},
                                 {49, {28, 41}},
                                 {50, {41, 36}},
                                 {51, {56, 52}},
                                 {52, {1, 0}},
                                 {53, {13}},
                                 {54, {}},
                                 {55, {2}},
                                 {56, {44, 25, 26, 20}},
                                 {57, {31, 21, 51, 14}},
                                 {58, {32, 26}},
                                 {59, {56, 43}}};
        return blocks::computeEvaluationSequence(graph);
    };

    BENCHMARK("120 nodes, 240 edges") {
        blocks::graph_t graph = {{0, {96}},
                                 {1, {5, 2}},
                                 {2, {118, 1}},
                                 {3, {31, 31, 73}},
                                 {4, {}},
                                 {5, {}},
                                 {6, {89}},
                                 {7, {28, 61}},
                                 {8, {}},
                                 {9, {16}},
                                 {10, {}},
                                 {11, {43, 103, 90, 49, 78}},
                                 {12, {58}},
                                 {13, {67, 77}},
                                 {14, {87, 110}},
                                 {15, {102, 27}},
                                 {16, {89, 99}},
                                 {17, {33, 77}},
                                 {18, {106, 100, 84}},
                                 {19, {49}},
                                 {20, {1, 116}},
                                 {21, {20}},
                                 {22, {75}},
                                 {23, {5}},
                                 {24, {60}},
                                 {25, {41, 114}},
                                 {26, {9}},
                                 {27, {118}},
                                 {28, {69}},
                                 {29, {}},
                                 {30, {5, 43}},
                                 {31, {78, 41, 42, 90, 68}},
                                 {32, {102, 67, 53}},
                                 {33, {}},
                                 {34, {32, 0}},
                                 {35, {85, 73}},
                                 {36, {49}},
                                 {37, {25, 7, 117}},
                                 {38, {}},
                                 {39, {42}},
                                 {40, {23, 105, 114}},
                                 {41, {48, 16, 43, 8}},
                                 {42, {39, 78, 18}},
                                 {43, {}},
                                 {44, {117, 78}},
                                 {45, {97, 63}},
                                 {46, {26, 106, 76}},
                                 {47, {9, 91, 103}},
                                 {48, {28, 34, 89}},
                                 {49, {78, 51, 68}},
                                 {50, {47, 14}},
                                 {51, {94, 94, 70, 27, 53}},
                                 {52, {56, 16}},
                                 {53, {76}},
                                 {54, {18}},
                                 {55, {45, 65}},
                                 {56, {}},
                                 {57, {94}},
                                 {58, {79}},
                                 {59, {81, 45, 23, 71, 106, 25, 51, 37}},
                                 {60, {}},
                                 {61, {108, 63, 1}},
                                 {62, {48}},
                                 {63, {16, 25, 52, 79, 44, 84}},
                                 {64, {14, 119, 69, 41}},
                                 {65, {100, 77, 69, 108, 75}},
                                 {66, {40, 14}},
                                 {67, {6, 15, 115}},
                                 {68, {21}},
                                 {69, {87, 95}},
                                 {70, {69, 103, 14}},
                                 {71, {22, 72, 30, 60}},
                                 {72, {94, 94}},
                                 {73, {75}},
                                 {74, {112, 35}},
                                 {75, {10}},
                                 {76, {3, 29, 36}},
                                 {77, {46, 83}},
                                 {78, {100, 101, 101}},
                                 {79, {21, 88, 8}},
                                 {80, {23}},
                                 {81, {6}},
                                 {82, {63}},
                                 {83, {99, 99, 7, 67}},
                                 {84, {71, 19}},
                                 {85, {59, 15}},
                                 {86, {11, 32, 14}},
                                 {87, {7}},
                                 {88, {101, 57, 89}},
                                 {89, {}},
                                 {90, {86, 88}},
                                 {91, {69}},
                                 {92, {6, 26, 115, 13, 31}},
                                 {93, {61, 98}},
                                 {94, {31, 62}},
                                 {95, {35}},
                                 {96, {117}},
                                 {97, {}},
                                 {98, {50, 74, 76}},
                                 {99, {27, 6, 55, 64, 45}},
                                 {100, {45}},
                                 {101, {}},
                                 {102, {22}},
                                 {103, {72}},
                                 {104, {65, 23}},
                                 {105, {}},
                                 {106, {39, 54, 70, 88}},
                                 {107, {47, 113, 72, 97}},
                                 {108, {46, 20}},
                                 {109, {61}},
                                 {110, {14}},
                                 {111, {80}},
                                 {112, {67}},
                                 {113, {108}},
                                 {114, {60, 92, 89}},
                                 {115, {3, 21}},
                                 {116, {40, 31, 0, 49, 9}},
                                 {117, {26, 10}},
                                 {118, {106, 72, 70, 36}},
                                 {119, {43, 77}}};

    };

    BENCHMARK("300 nodes, 600 edges") {
        blocks::graph_t graph = {{0, {}},
                                 {1, {3, 283, 268}},
                                 {2, {202, 213}},
                                 {3, {}},
                                 {4, {165, 65, 141}},
                                 {5, {70, 159}},
                                 {6, {57}},
                                 {7, {258, 169, 280, 24, 74, 162, 203}},
                                 {8, {}},
                                 {9, {294, 76, 275}},
                                 {10, {201, 58, 143}},
                                 {11, {}},
                                 {12, {247}},
                                 {13, {262}},
                                 {14, {71}},
                                 {15, {157}},
                                 {16, {33, 73, 96, 96}},
                                 {17, {63, 28, 135, 135, 224}},
                                 {18, {220, 271, 260}},
                                 {19, {124, 59}},
                                 {20, {}},
                                 {21, {243, 65, 119, 277, 49}},
                                 {22, {68}},
                                 {23, {26}},
                                 {24, {10, 248, 167}},
                                 {25, {151, 161, 56}},
                                 {26, {112}},
                                 {27, {141, 297}},
                                 {28, {}},
                                 {29, {32, 145, 155}},
                                 {30, {26, 281, 136, 98}},
                                 {31, {}},
                                 {32, {}},
                                 {33, {122, 131, 160}},
                                 {34, {146, 85, 213, 245}},
                                 {35, {83}},
                                 {36, {}},
                                 {37, {10, 65}},
                                 {38, {52, 73, 137}},
                                 {39, {255, 153, 126, 200}},
                                 {40, {133}},
                                 {41, {209, 116, 3, 89, 111}},
                                 {42, {25}},
                                 {43, {}},
                                 {44, {260, 115, 77, 239, 89}},
                                 {45, {49, 264, 41, 207}},
                                 {46, {137, 149}},
                                 {47, {19, 294}},
                                 {48, {182}},
                                 {49, {68, 119, 224, 55}},
                                 {50, {165}},
                                 {51, {238, 237, 260}},
                                 {52, {30}},
                                 {53, {78, 235, 194}},
                                 {54, {42}},
                                 {55, {199}},
                                 {56, {}},
                                 {57, {111}},
                                 {58, {256}},
                                 {59, {284}},
                                 {60, {65, 213, 144, 35}},
                                 {61, {}},
                                 {62, {110, 29}},
                                 {63, {199, 74, 85}},
                                 {64, {150, 18, 169}},
                                 {65, {51}},
                                 {66, {146, 9}},
                                 {67, {227, 257, 106}},
                                 {68, {118, 112, 207}},
                                 {69, {0, 39}},
                                 {70, {10}},
                                 {71, {}},
                                 {72, {}},
                                 {73, {282, 110}},
                                 {74, {142, 182, 67}},
                                 {75, {31, 69}},
                                 {76, {120, 257, 269, 263}},
                                 {77, {240, 19}},
                                 {78, {39, 58}},
                                 {79, {}},
                                 {80, {33, 95}},
                                 {81, {263, 10}},
                                 {82, {141, 88, 152}},
                                 {83, {84, 95}},
                                 {84, {172, 106}},
                                 {85, {15, 260}},
                                 {86, {94, 42, 84, 25}},
                                 {87, {97}},
                                 {88, {210}},
                                 {89, {18}},
                                 {90, {95, 185, 211, 67}},
                                 {91, {295}},
                                 {92, {}},
                                 {93, {11}},
                                 {94, {175}},
                                 {95, {}},
                                 {96, {112}},
                                 {97, {}},
                                 {98, {210, 142, 50, 160, 3}},
                                 {99, {82, 292}},
                                 {100, {266, 201}},
                                 {101, {4, 159, 66, 46, 224}},
                                 {102, {182, 213}},
                                 {103, {287, 211}},
                                 {104, {168, 150, 35}},
                                 {105, {}},
                                 {106, {144}},
                                 {107, {19}},
                                 {108, {259, 161, 2}},
                                 {109, {251, 107, 254}},
                                 {110, {106, 38, 4}},
                                 {111, {289, 8}},
                                 {112, {273}},
                                 {113, {139, 42}},
                                 {114, {165, 159, 15, 37, 222}},
                                 {115, {}},
                                 {116, {145, 54, 74}},
                                 {117, {83, 1}},
                                 {118, {212, 59}},
                                 {119, {177}},
                                 {120, {240, 15, 49}},
                                 {121, {227, 250, 1, 288, 32, 214}},
                                 {122, {194, 69, 277}},
                                 {123, {229, 25}},
                                 {124, {33, 13}},
                                 {125, {26}},
                                 {126, {156, 220}},
                                 {127, {48}},
                                 {128, {168, 63}},
                                 {129, {0}},
                                 {130, {74}},
                                 {131, {180, 192, 105}},
                                 {132, {}},
                                 {133, {91, 266}},
                                 {134, {189, 289, 37, 138}},
                                 {135, {263, 246, 93, 226, 84, 163}},
                                 {136, {45}},
                                 {137, {18}},
                                 {138, {88, 214, 256}},
                                 {139, {79, 164, 291}},
                                 {140, {}},
                                 {141, {208}},
                                 {142, {183, 147, 294}},
                                 {143, {}},
                                 {144, {4, 135}},
                                 {145, {161}},
                                 {146, {13}},
                                 {147, {200}},
                                 {148, {3, 260}},
                                 {149, {34}},
                                 {150, {288}},
                                 {151, {141, 238}},
                                 {152, {84}},
                                 {153, {100, 142}},
                                 {154, {205, 291, 248, 265}},
                                 {155, {15, 259, 236}},
                                 {156, {164}},
                                 {157, {173, 72}},
                                 {158, {241, 143, 131}},
                                 {159, {293, 40}},
                                 {160, {41, 185}},
                                 {161, {274, 70}},
                                 {162, {286}},
                                 {163, {254, 90, 25}},
                                 {164, {23, 281, 197, 294, 271, 29, 34}},
                                 {165, {254, 99}},
                                 {166, {}},
                                 {167, {178, 182}},
                                 {168, {233, 97, 66, 33, 265}},
                                 {169, {47, 273, 199, 55}},
                                 {170, {245}},
                                 {171, {203}},
                                 {172, {}},
                                 {173, {76, 39, 60}},
                                 {174, {62, 68, 182}},
                                 {175, {243, 249}},
                                 {176, {288, 90}},
                                 {177, {297, 247}},
                                 {178, {200}},
                                 {179, {12}},
                                 {180, {}},
                                 {181, {17, 38, 214}},
                                 {182, {274, 159}},
                                 {183, {69, 121, 280}},
                                 {184, {24, 229, 238}},
                                 {185, {112}},
                                 {186, {124, 168, 265}},
                                 {187, {69, 68, 291}},
                                 {188, {}},
                                 {189, {188, 46, 273}},
                                 {190, {153, 75}},
                                 {191, {229}},
                                 {192, {41, 19, 9, 184, 153}},
                                 {193, {}},
                                 {194, {83}},
                                 {195, {93}},
                                 {196, {224}},
                                 {197, {227, 265, 286}},
                                 {198, {81}},
                                 {199, {178, 184}},
                                 {200, {125}},
                                 {201, {290, 171}},
                                 {202, {}},
                                 {203, {99, 0, 285, 292, 235}},
                                 {204, {31}},
                                 {205, {69}},
                                 {206, {296, 27, 114}},
                                 {207, {172, 59}},
                                 {208, {70, 161}},
                                 {209, {286, 255, 295}},
                                 {210, {}},
                                 {211, {}},
                                 {212, {180}},
                                 {213, {108, 171, 100, 91}},
                                 {214, {}},
                                 {215, {32, 32}},
                                 {216, {101, 27}},
                                 {217, {172, 126}},
                                 {218, {236, 185, 31, 24, 248}},
                                 {219, {272, 239, 190}},
                                 {220, {28, 244, 251}},
                                 {221, {119, 205}},
                                 {222, {1, 149}},
                                 {223, {269}},
                                 {224, {3, 85, 253}},
                                 {225, {181}},
                                 {226, {52, 108, 212, 87}},
                                 {227, {69, 195, 14}},
                                 {228, {61, 87}},
                                 {229, {97, 147, 10, 156, 2}},
                                 {230, {192, 134}},
                                 {231, {25}},
                                 {232, {67, 114, 80}},
                                 {233, {}},
                                 {234, {}},
                                 {235, {95, 148, 73, 195}},
                                 {236, {}},
                                 {237, {66, 178, 56, 189, 240}},
                                 {238, {271, 91, 34}},
                                 {239, {219, 182, 251}},
                                 {240, {}},
                                 {241, {261, 229, 47}},
                                 {242, {106}},
                                 {243, {99, 85, 216, 239}},
                                 {244, {97, 258}},
                                 {245, {129, 33, 299}},
                                 {246, {215}},
                                 {247, {}},
                                 {248, {269, 166, 181}},
                                 {249, {271, 228, 57}},
                                 {250, {10, 66, 163}},
                                 {251, {}},
                                 {252, {94, 79, 192, 33}},
                                 {253, {207}},
                                 {254, {}},
                                 {255, {197}},
                                 {256, {117, 59, 274}},
                                 {257, {228, 104}},
                                 {258, {128, 131}},
                                 {259, {}},
                                 {260, {289}},
                                 {261, {112, 42}},
                                 {262, {299, 231, 32, 46, 85, 33}},
                                 {263, {216, 60}},
                                 {264, {59, 184, 92, 179}},
                                 {265, {228, 164, 91, 163}},
                                 {266, {92, 135}},
                                 {267, {43, 58}},
                                 {268, {111, 41}},
                                 {269, {211}},
                                 {270, {}},
                                 {271, {226, 66, 128}},
                                 {272, {283, 187, 259}},
                                 {273, {}},
                                 {274, {122, 36, 199}},
                                 {275, {177}},
                                 {276, {}},
                                 {277, {205, 273, 87, 137}},
                                 {278, {1}},
                                 {279, {}},
                                 {280, {24, 104, 143}},
                                 {281, {61, 177, 172}},
                                 {282, {295, 54}},
                                 {283, {183, 123, 150, 18}},
                                 {284, {123}},
                                 {285, {236}},
                                 {286, {15}},
                                 {287, {44, 146, 184}},
                                 {288, {244}},
                                 {289, {2, 161}},
                                 {290, {184, 91}},
                                 {291, {192, 225}},
                                 {292, {172}},
                                 {293, {229}},
                                 {294, {122, 56, 233, 272, 218}},
                                 {295, {257}},
                                 {296, {149}},
                                 {297, {49, 79}},
                                 {298, {95}},
                                 {299, {166}}};
        return blocks::computeEvaluationSequence(graph);
    };
}
*/