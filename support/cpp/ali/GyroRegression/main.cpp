/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Ali Baharev
*/

#include <iostream>
#include <memory>
#include <stdexcept>
#include "ErrorCodes.hpp"
#include "Optimizer.hpp"
#include "DataImporter.hpp"
#include "RotationMatrix.hpp"

using namespace std;

using namespace gyro;

void print_solution(const double* x, const Optimizer& opt) {

	for (int i=0; i<opt.n_vars(); ++i)
		cout << "x[" << i << "]\t" << x[i] << endl;

	cout << endl;

	cout << "Error in g (in m/s^2) : " << opt.error_in_g() << endl;
}

void run_solver(const input& data) {

	Optimizer opt(data);

	const double* const x = opt.solution();

	RotationMatrix rot(data, x);

	cout << "Error in g (in m/s^2) : " << opt.error_in_g() << endl;

	rot.dump_matrices();
}

int main(int argc, char* argv[]) {

	auto_ptr<const input> data(read_stdin());

	if (data.get()==0) {

		return ERROR_READING_INPUT;
	}

	try {

		run_solver(*data);

		data.reset();
	}
	catch (logic_error& e) {

		return ERROR_INITIALIZATION;
	}
	catch (runtime_error& e) {

		return ERROR_CONVERGENCE;
	}
	catch (...) {

		return ERROR_UNKNOWN;
	}

	return SUCCESS;

}

