#ifndef LINEARSYST_H_INCLUDED
#define LINEARSYST_H_INCLUDED

#include <vector>

#include "LinearCstr.h"

/*! \brief Stockage d'un système de contraintes linéaires. */

class LinearSyst: std::vector<LinearCstr>
{
private:
	typedef std::vector<LinearCstr> __internal;
public:
	using __internal::const_iterator;
	using __internal::empty;
	using __internal::begin;
	using __internal::end;
	using __internal::clear;
	using __internal::size;

    LinearSyst() {}

    LinearSyst(const LinearCstr &c);

/*! \brief Opérateur permettant l'ajout d'une contrainte dans le système.

    \param c La contrainte à ajouter.
    \return Une référence vers le système modifié. */

    LinearSyst &operator+(const LinearCstr &c);

/*! \brief Réécriture des contraintes.

	Utilisation d'un jeu réduit de comparateurs (>, >= et =). Les coefficients sont premiers entre eux
	après exécution de la méthode.
	\return faux si le système a été montré comme non satisfiable, vrai sinon. */

    bool harmonisation();

/*! \brief Suppression d'une variable dans le système.

    L'élimination se fait en utilisant l'algorithme de Fourier-Motzkin.
    \param v La variable à éliminer.
    \return Un booléen indiquant si l'élimination a (true), ou non (false), ajouté des solutions entières.  */

    bool elim_var(const Variable &v);

/*! \brief Susbstitution de la variable \a v par l'expression \a l dans toutes les contraintes du système. */

	void subst(const Variable &v, const LinearExpr &l);

};

std::ostream &operator<<(std::ostream &os, const LinearSyst &s);

#endif // LINEARSYST_H_INCLUDED
