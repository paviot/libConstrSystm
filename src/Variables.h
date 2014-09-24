#ifndef __VARIABLES__

#define __VARIABLES__

#include <string>
#include <iostream>

#define VARUNSET -1

class LinearCstr;

/*!	\brief Définition basique d'une variable.

	Une variable doit offrir une interface comprenant un opérateur d'écriture <<. */

class Variable
{
private:

/*! \var string name
	\brief Le nom de la variable */

	std::string name;

/*! \var int id
	\brief Le rang de la variable.

	Utile pour utiliser les bibliothèques de programmation linéaire. */

	int id;

public:
	Variable(const std::string &s=""): name(s), id(VARUNSET) {}

/*! \brief La méthode pour changer le nom. */

	void set_name(const std::string &n);

/*! \brief La méthode pour lire le nom. */

	const std::string &get_name() const;

/*! \brief La méthode pour changer l'identitié. */

    void set_id(int i);

/*! \brief La méthode pour lire l'identitié. */

    int get_id() const;

/*! \brief Création d'une contrainte. */

    LinearCstr operator=(int i) const;

};

/*! L'opérateur d'impression d'une variable. */

std::ostream &operator<<(std::ostream &os, const Variable &v);

#endif
