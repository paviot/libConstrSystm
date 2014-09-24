#include <iostream>
#include <cassert>

#include "LinearExpr.h"
#include "LinearCstr.h"

LinearExpr::LinearExpr(const Variable *v, int c)
{
    assert(c!= 0);

    (*this)[v] = c;
}

LinearExpr::LinearExpr(const Variable &v)
{
	(*this)[&v] = 1;
}

int LinearExpr::get_coeff(const Variable &v) const
{
    int							res = 0;
    __internal::const_iterator	it;

    if ((it = this->find(&v)) != this->end())
        res = it->second;

    return res;
}

LinearExpr LinearExpr::operator+(const LinearExpr& l) const
{
    LinearExpr l1 = *this;

    for(__internal::const_iterator itl = l.begin(); itl != l.end(); ++itl)
    {
        __internal::iterator it;

        if ((it = l1.find(itl->first)) != l1.end())
        {
            it->second += itl->second;

            if (it->second == 0)
                l1.erase(it);
        }
        else
            l1[itl->first] = itl->second;
    }

    return l1;

}

LinearExpr LinearExpr::operator* (int c) const
{
    LinearExpr l1 = *this;

    for (__internal::iterator it = l1.begin(); it != l1.end(); ++it)
        it->second *= c;

    return l1;
}

LinearExpr LinearExpr::operator-(const LinearExpr& l) const
{
    return *this + l*(-1);
}

LinearExpr LinearExpr::subst(const Variable &v, const LinearExpr &l) const
{
	int res = get_coeff(v);

	if (res == 0)
		return *this;
	else
		return *this - res*v + res*l;
}

LinearCstr LinearExpr::operator=(int cst) const
{
    return LinearCstr(*this, LinearCstr::EQUAL, cst);
}

std::ostream &operator<< (std::ostream &os, const LinearExpr &l)
{
	if (l.empty())
		os << '0';
	else
	{
		bool suite = false;

		for (LinearExpr::const_iterator it = l.begin(); it != l.end(); ++it)
		{
			int c = it->second;

			if (suite)
				if (c >= 0)
					os << " + ";
				else
				{
					os << " - ";
					c *= -1;
				}
			else
			{
				if (c == -1)
				{
					os << '-';
					c = 1;
				}

				suite = true;
			}

			if (c != 1)
				os << c << '*';

			os << *(it->first);
		}
	}

	return os;
}

LinearExpr operator* (const Variable &v, int c)
{
    return LinearExpr(&v, c);
}

LinearExpr operator* (int c, const Variable &v)
{
    return LinearExpr(&v, c);
}

LinearExpr operator* (int c, const LinearExpr& l)
{
	return l*c;
}

LinearExpr operator+ (const Variable &v1, const Variable &v2)
{
	return LinearExpr(&v1, 1) + LinearExpr(&v2, 1);
}

LinearExpr operator+ (const Variable &v, const LinearExpr& l)
{
	return 1*v + l;
}

LinearExpr operator+ (const LinearExpr& l, const Variable &v)
{
	return 1*v + l;
}

LinearExpr operator- (const Variable &v, const LinearExpr& l)
{
	return 1*v - l;
}

LinearExpr operator- (const LinearExpr& l, const Variable &v)
{
	return -1*v + l;
}

LinearExpr operator-(const Variable &v1, const Variable &v2)
{
	return 1*v1 - 1*v2;
}


