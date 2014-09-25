#ifndef LPS_H_INCLUDED
#define LPS_H_INCLUDED

#include <vector>
#include <iostream>

#include "lpsolve/lp_lib.h"

#include "LinearExpr.h"
#include "LinearSyst.h"

// Valeur d'initialisation du champ solv de lps

#define NOTSOLVED -10

// Nombre de configurations actuellement connues

#define NBCONFIG 2

/*! \brief La classe permettant de manipuler les systèmes de contraintes linéaires via lp_solve. */

class lps
{
private:

friend std::ostream &operator<<(std::ostream &os, const lps &l);

/*! \var lprec *lp
    \brief Le programme linéaire manipulé par lp_solve. */

    lprec *lp;

/*! \var int solv
	\brief Le type de résultat obtenu lors du dernier appel à mysolve.

	S'il n'y a pas eu d'appel à mysolve depuis la création, la variable est initialisée à NOTSOLVED. */

    int solv;

/*! \var REAL *sol
	\brief La solution au dernier appel de mysolve. */

	REAL *sol;

/*! \brief Permet de retourner le tableau des indices \a num et le tableau des valeurs \a val, représentant
	sous forme compacte une contrainte pour lp_solve. */

    struct lpCstr
    {
    	int		*num;
    	REAL	*val;
    };

/*! \brief Transforme l'expression linéaire \a l en son équivalent compacte pour lp_solve. */

	lpCstr *translate_expr(const LinearExpr &l) const;

public:

/*! \brief Construction d'un système ayant \a nbc contraintes et \a nbv variables.

	\param nbc Le nombre de contraintes.
	\param nbv Le nombre de variables. */

    lps(unsigned int nbc, unsigned int nbv);

/*! \brief Constructeur par copie.

	Le champ \a sol n'est pas copié. */

    lps(const lps &l);

    ~lps();

/*! \brief Après l'utilisation de cette méthode, \a lp contient une traduction au format lp_solve de \a s.

	\param s Le système à transcrire dans le format lp_solve. */

    void translate_syst(const LinearSyst &s);

/*! \brief Définition de la fonction à optimiser.

	\param l L'expresison linéaire à optimiser.

	Dans le cas de lp_solve, il semblerait plus efficace d'appeler cette fonction avant lps::translate_syst. */

	void translate_obj(const LinearExpr &l);

/*! \brief Indique que l'on souhaite maximiser la fonction lors de l'optimisaiton. */

	void set_max();

/*! \brief Indique que l'on souhaite minimiser la fonction lors de l'optimisaiton. */

	void set_min();

/*! \brief Indique que les variables sont entières. */

	void set_integer();

/*! \brief Donne la borne inférieure d'une variable. */

	void set_lower_bound(const Variable &v, REAL val);

/*! \brief Donne la borne supérieure d'une variable. */

	void set_upper_bound(const Variable &v, REAL val);

/*! \brief Met la base de départ à la base par défaut du simplex.

	Utilisé quand on tombe sur un problème numérique ou une impossibilité (du style
	absence de solution). */

	void set_default_basis();

/*! \brief Définition des différentes configurations utilisées.

	Il s'agit de définir les stratégies pour l'algorithme de branch-and-bound,
	pour le pivot... */

	void set_config(unsigned int i);

/*! \brief Fixe un temps limite aux calculs sur le système de contraintes. */

	void set_my_timeout(long t);

/*! \brief Calcule une base de départ à partir d'une solution connue.

	La bses de départ est ensuite donnée à \a lp. */

	void compute_basis(const std::vector<unsigned int> &sol);

/*! \brief Lance lp_solve.

	\return A value explaining the kind of result found. */

	void mysolve();

/*! \brief Indique si le dernier calcul a conduit à un résultat non borné. */

	bool is_unbounded() const;

/*! \brief Indique si le dernier calcul a conduit à un résultat optimal. */

	bool is_optimal() const;

/*! \brief Indique si le dernier calcul n'est pas faisable. */

	bool is_infeasible() const;

/*! \brief Indique si le dernier calcul n'a pas eu assez de mémoire. */

	bool is_out_of_memory() const;

/*! \brief Indique si le dernier calcul a rencontré un pb lors du calcul. */

	bool is_num_failure() const;

/*! \brief Indique si le dernier calcul a été interrompu après avoir trouvé une solution entière. */

	bool is_sub_optimal() const;

/*! \brief Indique si le dernier calcul a été interrompu sans avoir trouvé de solution. */

	bool is_time_out() const;

/*! \brief Retourne la valeur associée à la variable \a v dans le résultat calculé par mysolve().

	\param v La variable dont on veut le résultat.
	\return La valeur associée à la variable.
	\pre L'appel à mysolve a retourné un résultat optimal. */

	long double get_result(const Variable &v);

/*! \brief Retourne la valeur obtenue par le dernier mysolve() pour la fonction à optimizer.

	\return La valeur associée à la fonction objectif.
	\pre L'appel à mysolve a retourné un résultat optimal. */

	long double get_obj() const;

};

/*! \brief L'opérateur permettant d'afficher ce qui a été vraiment entré dans lp_solve. */

std::ostream &operator<<(std::ostream &os, const lps &l);



#endif // LPS_H_INCLUDED
