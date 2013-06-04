#include "ibex.h"

using namespace std;
using namespace ibex;

double convert(const char* argname, const char* arg) {
	char* endptr;
	double val = strtod(arg,&endptr);
	if (endptr!=arg+strlen(arg)*sizeof(char)) {
		stringstream s;
		s << "\"" << argname << "\" must be a real number";
		ibex_error(s.str().c_str());
	}
	return val;
}

int main(int argc, char** argv) {

	try {

		// check the number of arguments
	/*	if (argc<5) {
			ibex_error("usage: ARToptimizer filename prec goal_prec timelimit");
		}
*/
		// Load a system of equations
		System sys(argv[1]);

		cout << " " << argv[1] << " &" ;
/*
		double prec       = convert("prec",argv[2]);
		double goal_prec  = convert("goal_prec",argv[3]);  // the required precision for the objective
		double time_limit = convert("timelimit",argv[4]);
 */
		double prec       = 1.e-6;
		double goal_prec  = 1.e-6;
		double time_limit = 30;

		if (!sys.goal) {
			ibex_error(" input file has not goal (it is not an optimization problem).");
		}

		// Build the default optimizer
		ARTOptimizer o(sys,prec,goal_prec);

		// This option limits the search time
		o.timeout=time_limit;

		// This option prints each better feasible point when it is found
		o.trace=0;

		// display solutions with up to 12 decimals
		cout.precision(12);

		// Search for the optimum
		o.optimize(sys.box);

		// Report some information (computation time, etc.)
		//o.report();
		o.report_perf();

		return 0;

	}
	catch(ibex::SyntaxError& e) {
		cout << e << endl;
	}
}
