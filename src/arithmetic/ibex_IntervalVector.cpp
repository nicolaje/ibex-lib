/* ============================================================================
 * I B E X - Interval Vector definition
 * ============================================================================
 * Copyright   : Ecole des Mines de Nantes (FRANCE)
 * License     : This program can be distributed under the terms of the GNU LGPL.
 *               See the file COPYING.LESSER.
 *
 * Author(s)   : Gilles Chabert
 * Created     : Dec 05, 2011
 * ---------------------------------------------------------------------------- */

#include "ibex_IntervalVector.h"
#include <vector>
#include <stdlib.h>
#include <math.h>

namespace ibex {


IntervalVector::IntervalVector(int n) : n(n), vec(new Interval[n]) {
	assert(n>=1);
	for (int i=0; i<n; i++) vec[i]=Interval::ALL_REALS;
}

IntervalVector::IntervalVector(int n, const Interval& x) : n(n), vec(new Interval[n]) {
	assert(n>=1);
	for (int i=0; i<n; i++) vec[i]=x;
}

IntervalVector::IntervalVector(const IntervalVector& x) : n(x.n), vec(new Interval[x.n]) {
	for (int i=0; i<n; i++) vec[i]=x[i];
}

IntervalVector::IntervalVector(int n, double bounds[][2]) : n(n), vec(new Interval[n]) {
	for (int i=0; i<n; i++)
		vec[i]=Interval(bounds[i][0],bounds[i][1]);
}

IntervalVector::IntervalVector(const Vector& x) : n(x.size()), vec(new Interval[n]) {
	for (int i=0; i<n; i++) vec[i]=x[i];
}

IntervalVector::~IntervalVector() {
	delete[] vec;
}

const Interval& IntervalVector::operator[](int i) const {
	assert(i>=0 && i<n);
	return vec[i];
}

Interval& IntervalVector::operator[](int i) {
	assert(i>=0 && i<n);
	return vec[i];
}

void IntervalVector::set_empty() {
	// warning: do not insert this test:
	//	 if (isEmpty()) return;
	// because we call set_empty() from set(...) and the first component
	// may be empty in an intermediate state

	//for (int i=0; i<size(); i++)
	//	(*this)[i]=Interval::EMPTY_SET;
	(*this)[0]=Interval::EMPTY_SET;
}

void IntervalVector::resize(int n2) {
	assert(n2>=1);
	assert((vec==NULL && n==0) || (n!=0 && vec!=NULL));

	if (n2==size()) return;

	Interval* newVec=new Interval[n2];
	int i=0;
	for (; i<size() && i<n2; i++)
		newVec[i]=vec[i];
	for (; i<n2; i++)
		newVec[i]=Interval::ALL_REALS;
	if (vec!=NULL) // vec==NULL happens when default constructor is used (n==0)
		delete[] vec;

	n   = n2;
	vec = newVec;
}

IntervalVector IntervalVector::subvector(int start_index, int end_index) {
	assert(!is_empty());
	assert(end_index>=0 && start_index>=0);

	assert(end_index<size() && start_index<=end_index);
	//throw NonRecoverableException("Invalid indices for IntervalVector::subvector");

	IntervalVector v(end_index-start_index+1);
	int j=0;
	for (int i=start_index; i<=end_index; i++) {
		v[j++]=(*this)[i];
	}
	return v;
}

IntervalVector& IntervalVector::operator=(const IntervalVector& x) {
	assert(size()==x.size()); // throw InvalidIntervalVectorOp("Cannot set a IntervalVector to a IntervalVector with different dimension");

	if (x.is_empty())
		set_empty();
	else
		// don't use "set(...)" because the test "is_empty()" called inside
		// may return prematurely in case "this" is empty.
		// use physical copy instead:
		for (int i=0; i<size(); i++)
			(*this)[i]=x[i];

	return *this;
}

IntervalVector& IntervalVector::operator&=(const IntervalVector& x)  {
	// dimensions are non zero henceforth
	if (size()!=x.size()) throw InvalidIntervalVectorOp("Cannot intersect IntervalVectores with different dimensions");

	if (is_empty()) return *this;
	if (x.is_empty()) { set_empty(); return *this; }

	for (int i=0; i<size(); i++) {
		(*this)[i] &= x[i];
		if ((*this)[i].is_empty()) {
			set_empty();
			return *this;
		}
	}
	return *this;
}

IntervalVector& IntervalVector::operator|=(const IntervalVector& x)  {
	// dimensions are non zero henceforth
	if (size()!=x.size()) throw InvalidIntervalVectorOp("Cannot make the hull of IntervalVectores with different dimensions");

	if (x.is_empty()) return *this;
	if (is_empty()) { *this=x; return *this; }

	for (int i=0; i<size(); i++) {
		(*this)[i] |= x[i];
	}
	return *this;
}

bool IntervalVector::operator==(const IntervalVector& x) const {
	if (n!=x.size()) return false;
	if (is_empty() || x.is_empty()) return is_empty() && x.is_empty();
	for (int i=0; i<n; i++)
		if ((*this)[i]!=(x[i])) return false;
	return true;
}

Vector IntervalVector::lb() const {
	assert(!is_empty());

	Vector l(size());
	for (int i=0; i<size(); i++) {
		l[i]=(*this)[i].lb();
	}
	return l;
}

Vector IntervalVector::ub() const {
	assert(!is_empty());

	Vector u(size());
	for (int i=0; i<size(); i++) {
		u[i]=(*this)[i].ub();
	}
	return u;
}

Vector IntervalVector::mid() const {
	assert(!is_empty());

	Vector mV(size());
	for (int i=0; i<size(); i++) {
		mV[i]=(*this)[i].mid();
	}
	return mV;
}

bool IntervalVector::is_flat() const {
	if (is_empty()) return true;
	for (int i=0; i<size(); i++)
		if ((*this)[i].is_degenerated()) // don't use diam() because of roundoff
			return true;
	return false;
}

bool IntervalVector::contains(const Vector& x) const {
  for (int i=0; i<size(); i++)
    if (!(*this)[i].contains(x[i])) return false;
  return true;
}

bool IntervalVector::is_unbounded() const {
  if (is_empty()) return false;
  for (int i=0; i<size(); i++)
    if ((*this)[i].is_unbounded()) return true;
  return false;
}

bool IntervalVector::is_subset(const IntervalVector& x) const {
  if (is_empty()) return true;
  for (int i=0; i<size(); i++)
    if (!(*this)[i].is_subset(x[i])) return false;
  return true;
}

bool IntervalVector::is_strict_subset(const IntervalVector& x) const {
  if (is_empty() && !x.is_empty()) return true;
  if (x.is_empty()) return false;
  for (int i=0; i<size(); i++)
    if (!(*this)[i].is_strict_subset(x[i])) return false;
  return true;
}

int IntervalVector::extr_diam_index(bool min) const {
	double d=(*this)[0].diam();
	int selectedIndex=0;
	if (is_empty()) throw InvalidIntervalVectorOp("Diameter of an empty IntervalVector is undefined");
	for (int i=1; i<size(); i++) {
		double w=(*this)[i].diam();
		if (min? w<d : w>d) {
			selectedIndex=i;
			d=w;
		}
	}
	return selectedIndex;
}

std::ostream& operator<<(std::ostream& os, const IntervalVector& x) {
	if (x.is_empty()) return os << "empty vector";

	os << "(";
	for (int i=0; i<x.size(); i++)
		os << x[i] << (i<x.size()-1? " ; " : "");
	os << ")";
	return os;
}

double IntervalVector::volume() const {
	if ((*this)[0].is_unbounded()) return POS_INFINITY;
	if ((*this)[0].is_degenerated()) return 0;
	double vol=::log(((*this)[0]).diam());
	for (int i=1; i<size(); i++) {
		if ((*this)[i].is_unbounded()) return POS_INFINITY;
		if ((*this)[i].is_degenerated()) return 0;
		vol+=::log(((*this)[i]).diam());
	}
	return ::exp(vol);
}

double IntervalVector::perimeter() const {
	if ((*this)[0].is_unbounded()) return POS_INFINITY;
	double per=((*this)[0]).diam();
	for (int i=1; i<size(); i++) {
		if ((*this)[i].is_unbounded()) return POS_INFINITY;
		per+=((*this)[i]).diam();
	}
	return per;
}

double distance(const IntervalVector& x1, const IntervalVector& x2) {
	assert(x1.size()==x2.size());

	double max = ibex::distance(x1[0],x2[0]);
	for (int i=1; i<x1.size(); i++) {
		double cand = ibex::distance(x1[i],x2[i]);
		if (max<cand) max = cand;
	}
	return max;
}

double IntervalVector::rel_distance(const IntervalVector& x) const {
	double max = (*this)[0].rel_distance(x[0]);
	for (int i=1; i<size(); i++) {
		double cand = (*this)[i].rel_distance(x[i]);
		if (max<cand) max = cand;
	}
	return max;
}

namespace { // to create anonymous structure/functions

/** \brief Complementary of an Interval
 *
 * Compute the complementary of x. The result is (c1 union c2)
 */
void complI(const Interval& x, Interval& c1, Interval& c2) {
	if (x.is_empty() || x.is_degenerated()) { // x.is_empty() should not happen if called from compl()
		c1=Interval::ALL_REALS;
		c2=Interval::EMPTY_SET;
		return;
	}
	else {
		if (x.lb()>NEG_INFINITY) {
			c1=Interval(NEG_INFINITY,x.lb());
			if (x.ub()<POS_INFINITY)
				c2=Interval(x.ub(),POS_INFINITY);
			else
				c2=Interval::EMPTY_SET;
		} else if (x.ub()<POS_INFINITY) {
			c1=Interval(x.ub(),POS_INFINITY);
			c2=Interval::EMPTY_SET;
		} else {
			c1=c2=Interval::EMPTY_SET;
		}
	}
}

/** \brief x\y
 *
 */
void diffI(const Interval& x, const Interval& y, Interval& c1, Interval& c2) {
	complI(y,c1,c2);
	c1 &= x;
	if (c1.is_degenerated()) c1=Interval::EMPTY_SET;
	c2 &= x;
	if (c2.is_degenerated()) c2=Interval::EMPTY_SET;

	if (c1.is_empty()) {
		c2=c1;
		c2=Interval::EMPTY_SET;
	}
}

} // end namespace


int IntervalVector::diff(const IntervalVector& y, IntervalVector*& result) const {
	const int n=size();
	const IntervalVector& x=*this;
	IntervalVector *tmp = new IntervalVector[2*n]; // in the worst case, there is 2n boxes
	Interval c1, c2;
	int b=0;
	if (y.is_empty()) {
		tmp[b].resize(n);
		tmp[b]=x; // copy of this
		b++;
	} else {
		for (int var=0; var<n; var++) {

			diffI(x[var],y[var],c1,c2);

			if (!c1.is_empty()) {
				tmp[b].resize(n);
				IntervalVector& v=tmp[b++];
				for (int i=0; i<var; i++) v[i]=y[i];
				v[var]=c1;
				for (int i=var+1; i<n; i++) v[i]=x[i];

				if (!c2.is_empty()) {
					tmp[b].resize(n);
					IntervalVector& v=tmp[b++];
					for (int i=0; i<var; i++) v[i]=y[i];
					v[var]=c2;
					for (int i=var+1; i<n; i++) v[i]=x[i];
				}
			}
		}
	}

	if (b==0) {
		result = new IntervalVector[1];
		result[0].resize(n);
		result[0].set_empty();
		b=1;
	} else {
		result=new IntervalVector[b];
		for (int i=0; i<b; i++) {
			result[i].resize(n);
			result[i]=tmp[i];
		}
	}
	delete[] tmp;

	return b;
}

int IntervalVector::complementary(IntervalVector*& result) const {
	return IntervalVector(size()).diff(*this,result);
}

IntervalVector IntervalVector::random() const {
	assert(!is_empty());

	IntervalVector b(size());
	for (int i=0; i<size(); i++) {
		const Interval& xi=(*this)[i];
		// get a random number in [-0.5,0.5]
		double r=rand()/(double) RAND_MAX -0.5;
		// pick a point in the domain of the ith variable
		double p = xi.mid() + r*xi.diam();
		// watch dog
		if (p<xi.lb()) p=xi.lb();
		else if (p>xi.ub()) p=xi.ub();
		b[i]=p;
	}
	return b;
}

bool proj_add(const IntervalVector& y, IntervalVector& x1, IntervalVector& x2) {
	x1 &= y-x2;
	x2 &= y-x1;
	return x1.is_empty() || x2.is_empty();
}

bool proj_sub(const IntervalVector& y, IntervalVector& x1, IntervalVector& x2) {
	x1 &= y+x2;
	x2 &= x1-y;
	return x1.is_empty() || x2.is_empty();
}

bool proj_mul(const IntervalVector& y, Interval& x1, IntervalVector& x2) {
	assert(y.size()==x2.size());

	for (int i=0; i<x2.size(); i++)
		if (!proj_mul(y[i], x1, x2[i])) {
			x2.set_empty();
			return false;
		}
	return true;
}

bool proj_mul(const Interval& z, IntervalVector& x, IntervalVector& y) {
	assert(x.size()==y.size());
	int n=x.size();

	if (n==1) {
		if (proj_mul(z,x[0],y[0])) return true;
		else { x.set_empty(); y.set_empty(); return false; }
	}

	Interval xy[n];  // xy[i] := x[i]y[i]
	Interval sum[n-1]; // sum[i] := x[0]y[0]+...x[i]y[i]

	// ------------- forward --------------------
	for (int i=0; i<n; i++) xy[i]=x[i]*y[i];
	sum[0]=xy[0];
	for (int i=1; i<n-1; i++) sum[i]=sum[i-1]+xy[i];

	// ------------- backward -------------------
	// (rem: we have n>=2)
	if (!proj_add(z, sum[n-2], xy[n-1])) { x.set_empty(); y.set_empty(); return false; }

	for (int i=n-3; i>=0; i--)
		if (!proj_add(sum[i+1],sum[i],xy[i+1])) { x.set_empty(); y.set_empty(); return false; }

	for (int i=0; i<n; i++)
		if (!proj_mul(xy[i],x[i],y[i])) { x.set_empty(); y.set_empty(); return false; }

	return true;
}

} // end namespace