#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cstddef>
#include "ra/henon.hpp"


using namespace ra::fractal_logic;

using std::cout, std::endl, std::size_t;

#define TEST_NAME "Check constructor"
TEMPLATE_TEST_CASE(TEST_NAME, "[constructor]", long double) {
    cout <<"#####Begin#### - "<< TEST_NAME <<endl;
	//using status = typename queue<TestType>::status; 

    henon_map<TestType> h;

    std::ofstream file;

    file.open("test2.pnm");
    //h.print_pnm_image(file);
    file.close();
}
#undef TEST_NAME



#define TEST_NAME "Test classical"
TEMPLATE_TEST_CASE(TEST_NAME, "[henon]", long double) {
    cout <<"#####Begin#### - "<< TEST_NAME <<endl;
	//using status = typename queue<TestType>::status; 

    henon_map<TestType> h;
    h.set_a(1.4);
    h.set_b(0.3);

    std::ofstream file;

    file.open("test2.pnm");
    //h.print_pnm_image(file);
    file.close();
}
#undef TEST_NAME

#define TEST_NAME ""
TEMPLATE_TEST_CASE(TEST_NAME, "[point]", long double) {
    cout <<"#####Begin#### - "<< TEST_NAME <<endl;
	//using status = typename queue<TestType>::status; 

    henon_map<TestType> h;
    h.set_a(1.4);
    h.set_b(0.3);

    std::ofstream file;

    file.open("classical.pnm");
    //h.print_pnm_image(file);
    file.close();
}
#undef TEST_NAME

/*#define TEST_NAME "Test point add/subtract"
TEMPLATE_TEST_CASE(TEST_NAME, "[henon]", long double) {
    cout <<"#####Begin#### - "<< TEST_NAME <<endl;
	//using status = typename queue<TestType>::status; 

    point<TestType> p1(1.0,1.0);
    point<TestType> p2(-1.0,-1.0);

    point<TestType> p3 = p1 + p2;

    CHECK(p3.x == 0.0);
    CHECK(p3.y == 0.0);

    p3 = p1 - p2;

    CHECK(p3.x == 2.0);
    CHECK(p3.y == 2.0);
}
#undef TEST_NAME*/
