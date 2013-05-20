//============================================================================
//                                  I B E X                                   
// File        : ibex_XNewton.cpp
// Author      : Ignacio Araya, Bertrand Neveu , Gilles Trombettoni
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : Jul 1, 2012
// Last Update : Nov 15, 2012
//============================================================================

#include "ibex_CtcARTiter.h"


namespace ibex {


// the constructor
CtcARTiter::CtcARTiter(const System& sys1, 	ctc_mode cmode,
		int max_iter1, int time_out1, double eps, double max_diam_box):
			CtcLinearRelaxation(sys1,cmode,max_iter1,time_out1,eps, max_diam_box)
{


}

CtcARTiter::~CtcARTiter() {


}


void CtcARTiter::contract (IntervalVector & box){
	if (box.max_diam() > max_diam_box) return; // is it necessary?  YES (BNE) Soplex can give false infeasible results with large numbers

	IntervalVector initbox(box);
	//returns the number of constraints in the linearized system

	try {
		int cont = linearization(box);
		if(cont<1)  return;

		IntervalVector epsilon(box.size(),Interval(-1,1));
		optimizer(epsilon);
		mylinearsolver->cleanConst();

		convert_back(box,epsilon);

	}
	catch(EmptyBoxException& ){
		box.set_empty(); // empty the box before exiting in case of EmptyBoxException
		throw EmptyBoxException();
	}

}


/*********generation of the linearized system*********/
int CtcARTiter::linearization( IntervalVector & box)  {

	// Update the bounds the variables
	mylinearsolver->initBoundVar(box);

	Affine2 af2;
	Vector rowconst(sys.nb_var);
	Interval ev(0.0);
	CmpOp op;
	int cont=0;
	LinearSolver::Status stat;

	// Create the linear relaxation of each constraint
	for(int ctr=0; ctr<sys.nb_ctr;ctr++){

		for (int i=0;i<sys.nb_var;i++)
			rowconst[i] = 0.0;

/////////////////:

		if (goal_ctr == ctr) {
			// TODO To check
			op = LEQ;
			ev =sys.ctrs[ctr].f.eval_affine2(box, af2);

			if (0.0<ev.lb())   throw EmptyBoxException();
			else if (ev.ub()<0.0) {
				for (int i=0;i<sys.nb_var;i++) {
					rowconst[i] = af2.val(i);
				}
				rowconst[goal_ctr] = -1.0;
				stat = mylinearsolver->addConstraint( rowconst, LEQ, (-(af2.err())).ub() );
				if (stat==LinearSolver::OK) cont++;
			}
		}
		else {
			ev =sys.ctrs[ctr].f.eval_affine2(box, af2);
			op= sys.ctrs[ctr].op;

			if (af2.size()>0) {
				//the affine2 form is valid

				switch (op) {
				case LEQ:
					if (0.0==ev.lb())   throw EmptyBoxException();
				case LT: {
					if (0.0<ev.lb())   throw EmptyBoxException();
					else if (0.0<ev.ub()) {
						for (int i=0;i<sys.nb_var;i++) {
							rowconst[i] = af2.val(i+1);
						}
						stat = mylinearsolver->addConstraint( rowconst, LEQ, (Interval::ONE*af2.err()-af2.val(0)).ub() );
						if (stat==LinearSolver::OK) cont++;
					}
					break;
				}
				case GEQ:
					if (ev.ub()==0.0)   throw EmptyBoxException();
				case GT: {
					if (ev.ub()<0.0)   throw EmptyBoxException();
					else if (ev.lb()<0.0) {
						for (int i=0;i<sys.nb_var;i++) {
							rowconst[i] = af2.val(i);
						}
						stat = mylinearsolver->addConstraint( rowconst, GEQ, (Interval::ONE*af2.err()-af2.val(0)).lb() );
						if (stat==LinearSolver::OK) cont++;
					}
					break;
				}
				case EQ : {
					if (!ev.contains(0.0))   throw EmptyBoxException();
					else  {
						for (int i=0;i<sys.nb_var;i++) {
							rowconst[i] = af2.val(i+1);
						}
						stat = mylinearsolver->addConstraint( rowconst, GEQ, (Interval::ONE*af2.err()-af2.val(0)).lb() );
						if (stat==LinearSolver::OK) cont++;
						stat = mylinearsolver->addConstraint( rowconst, LEQ, (Interval::ONE*af2.err()-af2.val(0)).ub() );
						if (stat==LinearSolver::OK) cont++;
					}
					break;
					//		mylinearsolver->addConstraint( row1, GEQ, (-ev).lb() );
					break;
				}
				}
				///////////////////
			}
		}
	}
	return cont;

}


void CtcARTiter::convert_back(IntervalVector & box, IntervalVector & epsilon) {

	double t, t1, t2;
	for (int i=0;i<box.size();i++) {
		t = box[i].mid();
		t1 =(t-box[i]).ub();
		t2= (box[i]-t).ub();
		box[i] =( (t1>t2) ? t1 : t2 ) *epsilon[i];
		box[i] += t;
	}

}




}
