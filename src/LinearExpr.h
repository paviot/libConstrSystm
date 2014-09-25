#ifndef _LinearExpr__

#define _LinearExpr__

#include <map>

#include "Variables.h"

/* ---------------------------------------------------------------------------------------------- */
/*                           L'interface pour les expressions linéaires                           */
/* ---------------------------------------------------------------------------------------------- */

class LinearCstr;

/*! \brief L'interface pour les expressions linéaires. */

class LinearExpr: std::map<const Variable *, int>
{
private:
	typedef std::map<const Variable *, int> __internal;
public:
	using __internal::const_iterator;
	using __internal::empty;
	using __internal::begin;
	using __internal::end;
	using __internal::size;

	LinearExpr() {}

/*! \brief Le constructeur avec 2 variables.

    \pre \a c ne doit pas être nul. */

	LinearExpr(const Variable *v, int c);

/*! \brief Le contructeur qui transforme une variable en expression linéaire. */

	LinearExpr(const Variable &v);

/*! \brief Retourne la valeur du coefficient associé à la variable \a v.

    Retourne 0 si la variable n'apparait pas dans la contrainte linéaire. */

    int get_coeff(const Variable &v) const;

/*! \brief Opérateur d'addition. */

	LinearExpr operator+(const LinearExpr& l) const;

/*! \brief Multiplication d'une expression linéaire par un entier */

	LinearExpr operator* (int c) const;

/*! \brief Opérateur de soustraction. */

	LinearExpr operator-(const LinearExpr& l) const;

/*! \brief Substitution de la variable \a v par l'expression linéaire \a l.

	\param v La variable à substituer,
	\param l L'expression substituée à \a v.

	\return Une nouvelle expression reflétant la modification. */

	LinearExpr subst(const Variable &v, const LinearExpr &l) const;

/*! \brief Opérateur de création d'une contrainte à partir d'une expression linéaire et d'une constante. */

    LinearCstr operator=(int cst) const;

};

/* ---------------------------------------------------------------------------------------------- */
/*                                     Opérateurs externes                                        */
/* ---------------------------------------------------------------------------------------------- */

/*! \brief L'opérateur permettant d'écrire une expression linéaire sur le flux de sortie. */

std::ostream &operator<< (std::ostream &os, const LinearExpr &l);

/*! \brief Opérateur permettant de créer un monôme. */

LinearExpr operator* (const Variable &v, int c);

/*! \brief Opérateur permettant de créer un monôme. */

LinearExpr operator* (int c, const Variable &v);

/*! \brief Multiplication d'une expression linéaire par un entier */

LinearExpr operator* (int c, const LinearExpr& l);

/*! \brief Addition entre deux variables. */

LinearExpr operator+ (const Variable &v1, const Variable &v2);

/*! \brief Addition entre une variable et une expression linéaire. */

LinearExpr operator+ (const Variable &v, const LinearExpr& l);

/*! \brief Addition entre une variable et une expression linéaire. */

LinearExpr operator+ (const LinearExpr& l, const Variable &v);

/*! \brief Soustraction entre une variable et une expression linéaire. */

LinearExpr operator- (const Variable &v, const LinearExpr& l);

/*! \brief Sousstraction entre une variable et une expression linéaire. */

LinearExpr operator- (const LinearExpr& l, const Variable &v);

/*! \brief Soustranction entre 2 variables. */

LinearExpr operator-(const Variable &v1, const Variable &v2);

#endif

