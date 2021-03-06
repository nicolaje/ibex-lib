//============================================================================
//                                  I B E X                                   
// File        : Q-intersection contractor
// Author      : Gilles Chabert
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : Apr 30, 2012
// Last Update : Apr 30, 2012
//============================================================================

#include "ibex_CtcQInter.h"
#include "ibex_QInter.h"
#include "ibex_EmptyBoxException.h"

namespace ibex {

CtcQInter::CtcQInter(int n, const Array<Ctc>& list, int q) : list(list), n(n), q(q), boxes(list.size(), n) {

}

void CtcQInter::contract(IntervalVector& box) {
	Array<IntervalVector> refs(list.size());

	for (int i=0; i<list.size(); i++) {
		try {
			boxes[i]=box;
			list[i].contract(boxes[i]);
		} catch(EmptyBoxException&) {
			assert(boxes[i].is_empty());
		}
		refs.set_ref(i,boxes[i]);
	}

	box = qinter(refs,q);

	if (box.is_empty()) throw EmptyBoxException();
}

CtcQInter2::CtcQInter2(int n, const Array<Ctc>& list, int q) : list(list), n(n), q(q), boxes(list.size(), n) {

}

void CtcQInter2::contract(IntervalVector& box) {
	Array<IntervalVector> refs(list.size());

	for (int i=0; i<list.size(); i++) {
		try {
			boxes[i]=box;
			list[i].contract(boxes[i]);
		} catch(EmptyBoxException&) {
			assert(boxes[i].is_empty());
		}
		refs.set_ref(i,boxes[i]);
	}

	box = qinter2(refs,q);

	if (box.is_empty()) throw EmptyBoxException();
}

CtcQInterProjF::CtcQInterProjF(int n, const Array<Ctc>& list, int q) : list(list), n(n), q(q), boxes(list.size(), n) {

}

void CtcQInterProjF::contract(IntervalVector& box) {
	Array<IntervalVector> refs(list.size());

	for (int i=0; i<list.size(); i++) {
		try {
			boxes[i]=box;
			list[i].contract(boxes[i]);
		} catch(EmptyBoxException&) {
			assert(boxes[i].is_empty());
		}
		refs.set_ref(i,boxes[i]);
	}

	box = qinter_projf(refs,q);

	if (box.is_empty()) throw EmptyBoxException();
}

CtcQInterCoreF::CtcQInterCoreF(int n, const Array<Ctc>& list, int q) : list(list), n(n), q(q), boxes(list.size(), n) {

}

void CtcQInterCoreF::contract(IntervalVector& box) {
	Array<IntervalVector> refs(list.size());

	for (int i=0; i<list.size(); i++) {
		try {
			boxes[i]=box;
			list[i].contract(boxes[i]);
		} catch(EmptyBoxException&) {
			assert(boxes[i].is_empty());
		}
		refs.set_ref(i,boxes[i]);
	}

	box = qinter_coref(refs,q);

	if (box.is_empty()) throw EmptyBoxException();
}

} // end namespace ibex
