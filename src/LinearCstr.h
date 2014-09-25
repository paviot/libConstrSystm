#ifndef __LINEARCSTR__

#define __LINEARCSTR__

#include <iostream>

#include "LinearExpr.h"

/*! \brief Une contrainte lineaire.

    Il sagit d'une contrainte de la forme \a expr \a cp \a cst.
    Notez que l'opérateur d'égalité permettant de créer une contrainte
    de la forme : 3*x + 2*y = 5 est défini dans la classe LinearExpr.
    Notez aussi qu'il n'est pas possible de créer une telle égalité
    en utilisant l'écriture suivante : 5 = 3*x + 2*y.

    Pour que cela soit possible, l'opérateur d'affectation est défini
    avec les contraintes linéaires (voir LinearCstr.h). */

class LinearCstr
{
public:
/*! \brief Définition du type de comparateur. */

    enum Comp {
        EQUAL,		/*!< Egalité */
        INF,		/*!< Strictement inférieur */
        INFEQ,		/*!< Inférieur ou égale */
        SUP,		/*!< Strictement supérieur */
        SUPEQ		/*!< Supérieur ou égal */
    };

private:

/*! \var LinearExpr expr
	\brief L'expression linéaire de la contrainte. */

	LinearExpr	expr;

/*! \var Comp cp
	\brief La nature du comparateur de la contrainte linéaire. */

	Comp		cp;

/*! \var int cst
    \brief La constante de la contrainte. */

    int         cst;

public:
	LinearCstr(): cp(EQUAL), cst(0) {}

	LinearCstr(const LinearExpr &l, Comp comp, int c):expr(l), cp(comp), cst(c) {}

/*! \brief Retourne le coefficient de la variable \a v dans la contrainte. */

    int get_coeff(const Variable &v) const;

/*! \brief Retourne l'expression linéaire de la contrainte. */

    const LinearExpr &get_linexpr() const;

/*! \brief Retourne l'expression linéaire de la contrainte. */

    LinearCstr::Comp get_comparator() const;

/*! \brief Retourne la constante de la contrainte. */

    int get_constant() const;

/*! \brief L'opérateur d'addition de 2 contraintes linéaires. */

    LinearCstr operator+(const LinearCstr &c) const;

/*! \brief Ajout d'un entier à une contrainte. */

    LinearCstr operator+(int cst1) const;

/*! \brief Soustraction d'un entier à une contrainte. */

    LinearCstr operator-(int cst1) const;

/*! \brief Multiplication d'une contrainte par un entier. */

    LinearCstr operator*(int cst1) const;

/*! \brief Susbstitution de la variable \a v par l'expression \a l dans la contrainte. */

	void subst(const Variable &v, const LinearExpr &l);
};

/*! \brief L'opérateur d'affichage d'une contrainte. */

std::ostream &operator<<(std::ostream &os, const LinearCstr &c);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator<(const LinearExpr &l, int cst);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator<(int cst, const LinearExpr &l);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator<=(const LinearExpr &l, int cst);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator<=(int cst, const LinearExpr &l);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator>(const LinearExpr &l, int cst);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator>(int cst, const LinearExpr &l);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator>=(const LinearExpr &l, int cst);

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

LinearCstr operator>=(int cst, const LinearExpr &l);

/*! \brief Opérateur d'addition d'une contraite et d'un entier. */

LinearCstr operator+(int cst, const LinearCstr &c);

/*! \brief Opérateur de soustraction d'une contraite et d'un entier. */

LinearCstr operator-(int cst, const LinearCstr &c);

/*! \brief Opérateur de multiplication d'une contraite et d'un entier. */

LinearCstr operator*(int cst, const LinearCstr &c);

#endif
