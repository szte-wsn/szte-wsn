#ifndef OPTIMIZER_HPP_
#define OPTIMIZER_HPP_

#include <iosfwd>

struct input;

class Optimizer {

public:

	Optimizer(const input& data, std::ostream& os, bool verbose = false);

private:

};

#endif

