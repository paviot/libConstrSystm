#include <stdlib.h>
#include <iostream>

#include "LinearExpr.h"
#include "LinearCstr.h"

int LinearCstr::get_coeff(const Variable &v) const
{
    return expr.get_coeff(v);
}

const LinearExpr &LinearCstr::get_linexpr() const
{
    return expr;
}

LinearCstr::Comp LinearCstr::get_comparator() const
{
    return cp;
}

int LinearCstr::get_constant() const
{
    return cst;
}

LinearCstr LinearCstr::operator+(const LinearCstr &c) const
{
    if ((cp == c.cp) || (c.cp == EQUAL))
        return LinearCstr(expr + c.expr, cp, cst + c.cst);

    if (cp == EQUAL)
        return LinearCstr(expr + c.expr, c.cp, cst + c.cst);

/* Si on a 2 comparateurs de meme nature (strict ou non), ils sont forcément opposés. */

    if ((((cp == INFEQ) || (cp == SUPEQ)) && ((c.cp == INFEQ) || (c.cp == SUPEQ))) ||
        (((cp == INF) || (cp == SUP)) && ((c.cp == INF) || (c.cp == SUP))))
        return LinearCstr(expr - c.expr, cp, cst - c.cst);

/* Les comparateurs sont de natures différentes. Il faut donc retourner le strict.  */

    if (((cp == INFEQ) || (cp == INF)) && ((c.cp == INFEQ) || (c.cp == INF)))
        return LinearCstr(expr + c.expr, INF, cst + c.cst);

    if (((cp == SUPEQ) || (cp == SUP)) && ((c.cp == SUPEQ) || (c.cp == SUP)))
        return LinearCstr(expr + c.expr, SUP, cst + c.cst);

/* Les comparateurs sont de natures différents et opposés. */

    if ((cp == INFEQ) || (cp == INF))
        return LinearCstr(expr - c.expr, INF, cst - c.cst);
    else
        return LinearCstr(expr - c.expr, SUP, cst - c.cst);
}

LinearCstr LinearCstr::operator+(int cst1) const
{
    return LinearCstr(expr, cp, cst + cst1);
}

LinearCstr LinearCstr::operator-(int cst1) const
{
    return *this + (-1)*cst1;
}

LinearCstr LinearCstr::operator*(int cst1) const
{
    if (cst1 == 0)
        return LinearCstr(LinearExpr(), cp, 0);
    else if (cst1 > 0)
        return LinearCstr(cst1*expr, cp, cst1*cst);
    else switch (cp)
        {
            case EQUAL:
                return LinearCstr(cst1*expr, cp, cst1*cst);
            case INF:
                return LinearCstr(cst1*expr, SUP, cst1*cst);
            case INFEQ:
                return LinearCstr(cst1*expr, SUPEQ, cst1*cst);
            case SUP:
                return LinearCstr(cst1*expr, INF, cst1*cst);
            case SUPEQ:
                return LinearCstr(cst1*expr, INFEQ, cst1*cst);
            default:
                std::cout << "Unknown comparator" << std::endl;
                exit(1);
        }
}

void LinearCstr::subst(const Variable &v, const LinearExpr &l)
{
	expr = expr.subst(v, l);
}

std::ostream &operator<<(std::ostream &os, const LinearCstr &c)
{
    os << c.get_linexpr() << ' ';

    switch (c.get_comparator())
    {
        case LinearCstr::EQUAL:
            os << '=';
            break;
        case LinearCstr::INF:
            os << '<';
            break;
        case LinearCstr::INFEQ:
            os << "<=";
            break;
        case LinearCstr::SUP:
            os << '>';
            break;
        case LinearCstr::SUPEQ:
            os << ">=";
            break;
        default:
            std::cout << "Unknown comparator" << std::endl;
            exit(1);
    }

    os << ' ' << c.get_constant();

    return os;
}

LinearCstr operator<(const LinearExpr &l, int cst)
{
    return LinearCstr(l, LinearCstr::INF, cst);
}

LinearCstr operator<(int cst, const LinearExpr &l)
{
    return LinearCstr(l, LinearCstr::SUP, cst);
}

LinearCstr operator<=(const LinearExpr &l, int cst)
{
    return LinearCstr(l, LinearCstr::INFEQ, cst);
}

LinearCstr operator<=(int cst, const LinearExpr &l)
{
    return LinearCstr(l, LinearCstr::SUPEQ, cst);
}

LinearCstr operator>(const LinearExpr &l, int cst)
{
    return LinearCstr(l, LinearCstr::SUP, cst);
}

LinearCstr operator>(int cst, const LinearExpr &l)
{
    return LinearCstr(l, LinearCstr::INF, cst);
}

LinearCstr operator>=(const LinearExpr &l, int cst)
{
    return LinearCstr(l, LinearCstr::SUPEQ, cst);
}

LinearCstr operator>=(int cst, const LinearExpr &l)
{
    return LinearCstr(l, LinearCstr::INFEQ, cst);
}

LinearCstr operator+(int cst, const LinearCstr &c)
{
    return c + cst;
}

LinearCstr operator-(int cst, const LinearCstr &c)
{
    return c - cst;
}

LinearCstr operator*(int cst, const LinearCstr &c)
{
    return c*cst;
}
