//============================================================================
//                                  I B E X                                   
// File        : ibex_CtcUnion.cpp
// Author      : Gilles Chabert
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : May 14, 2012
// Last Update : May 14, 2012
//============================================================================

#include "ibex_CtcUnion.h"

namespace ibex {

CtcUnion::CtcUnion(const Array<Ctc>& list) : Ctc(list[0].nb_var), list(list) {
	for (int i=0; i<list.size(); i++) {
		assert(list[i].nb_var==nb_var);
	}
}

void CtcUnion::contract(IntervalVector& box) {
	IntervalVector savebox(box);
	IntervalVector result(IntervalVector::empty(box.size()));

	for (int i=0; i<list.size(); i++) {
		if (i>0) box=savebox;
		list[i].contract(box);
		result |= box;
	}
	box = result;
} // end namespace ibex

}