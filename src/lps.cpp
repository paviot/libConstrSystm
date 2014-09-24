#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>

#include "Variables.h"
#include "LinearExpr.h"
#include "LinearSyst.h"
#include "lps.h"

lps::lps(unsigned int nbc, unsigned int nbv): solv(NOTSOLVED), sol(NULL)
{
    lp = make_lp(nbc, nbv);

    if (lp == NULL)
    {
        std::cout << "Error: cannot initialize the linear system for lp_solve" << std::endl;
        exit(1);
    }

    set_add_rowmode(lp, FALSE);	// Si la construction est pénalisante, on peut améliorer en jouant
								// avec cette fonction
}

lps::lps(const lps &l)
{
	lp = copy_lp(l.lp);
	solv = l.solv;
	sol = NULL;
}

lps::~lps()
{
	delete sol;
	delete_lp(lp);
}

lps::lpCstr *lps::translate_expr(const LinearExpr &l) const
{
	int		nbv = 0;	// Nombre de variables de l'expression linéaire déjà traitées

	if (l.size() == 0)
	{
		std::cout << "Expression vide dans translate_expr" << std::endl;
		exit(1);
	}

	lpCstr *res = new lpCstr;
	res->num = new int[l.size()];;
	res->val = new REAL[l.size()];

	for (LinearExpr::const_iterator itl = l.begin(); itl != l.end(); ++itl)
	{
		if (itl->first->get_id() == VARUNSET)
		{
			std::cout << "Using variable with unset id" << std::endl;
			exit(1);
		}

		res->num[nbv] = itl->first->get_id();

		if (res->num[nbv] > get_Ncolumns(lp))
		{
			std::cout << "Variable used with an id superior to the total number of variables in the system" << std::endl;
			exit(1);
		}

		char name[itl->first->get_name().size() + 1];

		strcpy(name, itl->first->get_name().c_str());
		set_col_name(lp, res->num[nbv], name);
		res->val[nbv] = itl->second;
		++nbv;
	}

	return res;
}

void lps::translate_syst(const LinearSyst &s)
{
    if (s. empty())
    {
        std::cout << "Error: try to create an empty linear system" << std::endl;
        exit(1);
    }

    int nbcstr = 1; // La numérotation des contraintes commence à 1

    for (LinearSyst::const_iterator its = s. begin(); its != s.end(); ++its)
    {
        const LinearExpr &l = its->get_linexpr();

        lpCstr *l1 = translate_expr(l);

        if (!set_rowex(lp, nbcstr, l.size(), l1->val, l1->num))
        {
            std::cout << "Cannot set the left hand side for a constraint" << std::endl;
            exit(1);
        }

        delete[] l1->num;
        delete[] l1->val;
        delete l1;

        unsigned char res;

        switch (its->get_comparator())
        {
            case LinearCstr::EQUAL:
                res = set_constr_type(lp, nbcstr, EQ);
                break;
            case LinearCstr::INF:
                std::cout << "INF is not a comaprator supported by lp_solve" << std::endl;
                exit(1);
            case LinearCstr::INFEQ:
                res = set_constr_type(lp, nbcstr, LE);
                break;
            case LinearCstr::SUP:
                std::cout << "SUP is not a comaprator supported by lp_solve" << std::endl;
                exit(1);
            case LinearCstr::SUPEQ:
                res = set_constr_type(lp, nbcstr, GE);
                break;
            default:
                std::cout << "Unsupported operator in lp_solve translation" << std::endl;
                exit(1);
        }

        if (!res)
        {
            std::cout << "Cannot set the comparator for a constraint" << std::endl;
            exit(1);
        }

        if (!set_rh(lp, nbcstr, its->get_constant()))
        {
            std::cout << "Cannot set the right hand side of a constraint" << std::endl;
            exit(1);
        }
        ++nbcstr;
    }
}

void lps::translate_obj(const LinearExpr &l)
{
        lpCstr *l1 = translate_expr(l);

        if (!set_obj_fnex(lp, l.size(), l1->val, l1->num))
        {
            std::cout << "Cannot set the objective function" << std::endl;
            exit(1);
        }

        delete[] l1->num;
        delete[] l1->val;
        delete l1;
}

void lps::set_max()
{
	set_maxim(lp);
}

void lps::set_min()
{
	set_minim(lp);
}

void lps::set_integer()
{
	for (int i = 1; i <= get_Ncolumns(lp); ++i)
		set_int(lp, i, TRUE);
}

void lps::set_upper_bound(const Variable &v, REAL val)
{
	int id = v.get_id();

	if (id != -1)
	{
		if (!set_upbo(lp, id, val))
		{
			std::cout << "Internal error: cannot set the upper bound of a variable" << std::endl;
			exit(1);
		}
	}
	else
	{
		std::cout << "Internal error: cannot the upper bound of an unknown variable" << std::endl;
		exit(1);
	}
}

void lps::set_lower_bound(const Variable &v, REAL val)
{
	int id = v.get_id();

	if (id != -1)
	{
		if (!set_lowbo(lp, id, val))
		{
			std::cout << "Internal error: cannot set the upper bound of a variable" << std::endl;
			exit(1);
		}
	}
	else
	{
		std::cout << "Internal error: cannot the upper bound of an unknown variable" << std::endl;
		exit(1);
	}
}

void lps::set_default_basis()
{
	default_basis(lp);
}

void lps::set_config(unsigned int i)
{
	switch (i)
	{
		case 0:		//Configuration par défaut de la 5.5.0.12
			set_bb_rule(lp, NODE_PSEUDONONINTSELECT + NODE_GREEDYMODE + NODE_DYNAMICMODE + NODE_RCOSTFIXING);
			set_pivoting(lp, PRICER_DEVEX | PRICE_ADAPTIVE);
			break;
		case 1:		// Configuration utilisée par le calcul de bornes
			set_bb_rule(lp, NODE_FRACTIONSELECT | NODE_BRANCHREVERSEMODE);
			set_pivoting(lp, PRICER_FIRSTINDEX);
			break;
		default:
			std::cout << "Internal error: unknown lp_solve configuration" << std::endl;
			exit(1);
	}
}

void lps::set_my_timeout(long t)
{
	set_timeout(lp, t);
}

void lps::compute_basis(const std::vector<unsigned int> &sol)
{
	int *basis = new int[1 + get_Nrows(lp) + get_Ncolumns(lp)];
	REAL sol1[1+get_Ncolumns(lp)];

	if (sol.size() != (unsigned int)get_Ncolumns(lp))
	{
		std::cout << "Internal error: the given solution does not have the right number of elements" << std::endl;
		exit(1);
	}

	for (unsigned int i = 0; i < sol.size(); ++i)
		sol1[i+1] = sol[i];

	if (!guess_basis(lp, sol1, basis))
	{
		std::cout << "Error: cannot compute a basis" << std::endl;
		exit(1);
	}

	set_basis(lp, basis, TRUE);
	delete basis;
}

void lps::mysolve()
{
	set_verbose(lp, SEVERE);
	solv = solve(lp);
}

bool lps::is_unbounded() const
{
	return solv == UNBOUNDED;
}

bool lps::is_optimal() const
{
	return solv == OPTIMAL;
}

bool lps::is_infeasible() const
{
	return solv == INFEASIBLE;
}

bool lps::is_out_of_memory() const
{
	return solv == NOMEMORY;
}

bool lps::is_num_failure() const
{
	return solv == NUMFAILURE;
}

bool lps::is_sub_optimal() const{
	return solv == SUBOPTIMAL;
}

bool lps::is_time_out() const
{
	return solv == TIMEOUT;
}

long double lps::get_result(const Variable &v)
{
	if (solv == NOTSOLVED)
	{
		std::cout << "Internal error: cannot fetch a solution that has not been computed" << std::endl;
	}

	if (solv != OPTIMAL)
	{
		std::cout << "Internal error: cannot fetch a solution that is not optimal" << std::endl;
		exit(1);
	}

	if (sol == NULL)
	{
		sol = new REAL[get_Ncolumns(lp)];

		if (!get_variables(lp, sol))
		{
			std::cout << "Cannot obtain the result" << std::endl;
			exit(1);
		}
	}

	if (v.get_id() > get_Ncolumns(lp))
	{
		std::cout << "Variable " << v << " is not part of the solved system." << std::endl;
		exit(1);
	}

	return sol[v.get_id()];
}

long double lps::get_obj() const
{
	if (solv != OPTIMAL)
	{
		std::cout << "Cannot fetch a solution that is not optimal" << std::endl;
		exit(1);
	}

	return get_objective(lp);
}

std::ostream &operator<<(std::ostream &os, const lps &l)
{
	write_LP(l.lp, stdout);

	return os;
}
