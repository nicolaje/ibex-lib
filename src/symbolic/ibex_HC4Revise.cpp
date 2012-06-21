//============================================================================
//                                  I B E X                                   
// File        : HC4Revise Algorithm
// Author      : Gilles Chabert
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : Dec 31, 2011
// Last Update : 
//============================================================================

#include "ibex_HC4Revise.h"
#include "ibex_BasicDecorator.h"

namespace ibex {

void HC4Revise::proj(const Function& f, const Domain& y, Array<Domain>& x) {
	Eval().eval(f,x);
	*f.expr().deco.d = y;
	f.backward<HC4Revise>(*this);
	x = f.symbol_domains;
}

void HC4Revise::proj(const Function& f, const Domain& y, IntervalVector& x) {
	Eval().eval(f,x);
	*f.expr().deco.d = y;
	f.backward<HC4Revise>(*this);

	if (f.all_symbols_scalar())
		for (int i=0; i<f.nb_symbols(); i++) {
			x[i]=f.symbol_domains[i].i();
		}
	else
		load(x,f.symbol_domains);
}

void HC4Revise::proj(const Function& f, const Domain& y, ExprLabel** x) {
	Eval().eval(f,x);
	*f.expr().deco.d = y;
	f.backward<HC4Revise>(*this);
	for (int i=0; i<f.nb_symbols(); i++) {
		*(x[i]->d) = f.symbol_domains[i];
	}
}

void HC4Revise::vector_bwd(const ExprVector& v, ExprLabel** compL, const ExprLabel& y) {
	if (v.dim.is_vector()) {
		for (int i=0; i<v.length(); i++)
			if ((compL[i]->d->i() &= y.d->v()[i]).is_empty()) throw EmptyBoxException();
	}
	else {
		if (v.row_vector())
			for (int i=0; i<v.length(); i++)
				if ((compL[i]->d->v()&=y.d->m().col(i)).is_empty()) throw EmptyBoxException();
		else
			for (int i=0; i<v.length(); i++)
				if ((compL[i]->d->v()=y.d->m().row(i)).is_empty()) throw EmptyBoxException();
	}
}

} /* namespace ibex */