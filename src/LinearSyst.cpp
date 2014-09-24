#include <iostream>
#include <vector>

#include "Utilities.h"
#include "LinearCstr.h"
#include "LinearSyst.h"

LinearSyst::LinearSyst(const LinearCstr &c)
{
    this->push_back(c);
}

LinearSyst &LinearSyst::operator+(const LinearCstr &c)
{
    this->push_back(c);

    return *this;
}

bool LinearSyst::harmonisation()
{
	bool res = true;

    for(__internal::iterator it = this->begin(); it != this->end(); ++it)
    {
    	/* Mise en conformité des comparateurs */

        if ((it->get_comparator() == LinearCstr::INF) || (it->get_comparator() == LinearCstr::INFEQ))
            *it = (-1)*(*it);

		/* Mise en conformité des coefficients */

	/*	A FAIRE */
    }

	return res;
}

bool LinearSyst::elim_var(const Variable &v)
{
    // On cherche une égalité contenant v, si possible avec un coefficient 1

	__internal::const_iterator iteq = this->begin();

	for(;	(iteq != this->end()) &&														// Si on n'est pas à la fin, on continue
			((iteq->get_comparator() != LinearCstr::EQUAL) ||								// Si on n'a pas une égalité, on continue
			 ((iteq->get_comparator() == LinearCstr::EQUAL) && (iteq->get_coeff(v) == 0)));	// Si on a une égalité sans v, on continue
			++iteq)
		continue;

	__internal::const_iterator iteq1 = iteq;

	for(;	(iteq1 != this->end()) &&															// Si on n'est pas à la fin, on continue
			((iteq1->get_comparator() != LinearCstr::EQUAL) ||									// Si on n'a pas une égalité, on continue
			 ((iteq1->get_comparator() == LinearCstr::EQUAL) && (iteq1->get_coeff(v) != 1)));	// Si on a une égalité avec un coeff différent de 1 pour v, on continue
			++iteq1)
		continue;

	if (iteq1 != this->end())
		iteq = iteq1;

	if (iteq != this->end())
	{
		int 					cst_eq = iteq->get_constant();
		int						coeff_eq = iteq->get_coeff(v);
		bool					res = ((coeff_eq != 1) || (coeff_eq != -1));
		std::vector<LinearCstr>	tmp;
		LinearExpr				exp = iteq->get_linexpr() - iteq->get_coeff(v)*v;

		if (coeff_eq < 0)
		{
			cst_eq = -1*cst_eq;
			coeff_eq = -1*coeff_eq;
		}
		else
			exp = -1*exp;

		for(__internal::const_iterator it = this->begin(); it != this->end(); ++it)
		{
			int c = it->get_coeff(v);

			if (it != iteq)
			{
				if (c == 0)
					tmp.push_back(*it);
				else
					tmp.push_back(LinearCstr(coeff_eq*it->get_linexpr() + c*(exp - v), it->get_comparator(), coeff_eq*it->get_constant() - c*cst_eq));
			}
		}

		this->clear();

		for (unsigned int i = 0; i < tmp.size(); ++i)
			*this = *this + tmp[i];

		return res;
	}
	else	// On applique Fourier-Motzkin
	{
		/* On trie les contraintes suivant le coefficient de v. */

		std::vector<LinearCstr> plus, moins, zero;
		bool                    res = false;

		this->harmonisation();

		for(__internal::const_iterator it = this->begin(); it != this->end(); ++it)
		{
			int coeff = it->get_coeff(v);

			if (coeff == 0)
				zero.push_back(*it);
			else if (coeff < 0)
				moins.push_back(*it);
			else
				plus.push_back(*it);
		}

		this->clear();

		/* On recopie les contraintes non concernées. */

		for(__internal::const_iterator it = zero.begin(); it != zero.end(); ++it)
			this->push_back(*it);

		/* On additionne 2 à 2 chaque plus avec chaque moins. */

		for (__internal::const_iterator itp = plus.begin(); itp != plus.end(); ++itp)
		{
			int coeffp = itp->get_coeff(v);

			for (__internal::const_iterator itm = moins.begin(); itm != moins.end(); ++itm)
			{
				int coeffm = -1*(itm->get_coeff(v));
				int gc = gcd(coeffp, coeffm);

				res = (res || ((coeffp != 1) && (coeffm != 1)));
				this->push_back((coeffm/gc)*(*itp) + (coeffp/gc)*(*itm));
			}
		}

		return res;
	}
}

void LinearSyst::subst(const Variable &v, const LinearExpr &l)
{
	for (unsigned int i = 0; i < this->size(); ++i)
		(*this)[i].subst(v, l);
}

std::ostream &operator<<(std::ostream &os, const LinearSyst &s)
{
    for(LinearSyst::const_iterator it = s.begin(); it != s.end(); ++it)
        os << *it << std::endl;

    return os;
}
