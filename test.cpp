#include "shared_ptr.h"
#include "unique_ptr.h"
#include "vector.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <gtest/gtest.h>
using namespace std;

// int main() {
	// mystl::shared_ptr<std::string> pt(new string("hello"));
	// pt->push_back('a');
	// std::cout << *pt << std::endl;

	// mystl::unique_ptr<std::string> u_pt(new string("hello"));
	// u_pt->push_back('b');
	// std::cout << *u_pt << std::endl;
	// std::iterator_traits<mystl::vector<int>::iterator>::value_type m = 1;
	// cout << m << endl;

	// mystl::vector<int> vec(3, 0);
	// vec.reserve(6);
	// cout << vec.size() << endl;
	// mystl::vector<int> vec1(2, 1);
	// vec.insert(vec.begin(), vec1.begin(), vec1.end());
	// for (auto& num : vec)
	// 	cout << num << ' ';
	// cout << endl;

	// vec.emplace(vec.begin(), 3);
	// for (auto& num : vec)
	// 	cout << num << ' ';
	// cout << endl;
// }


int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}