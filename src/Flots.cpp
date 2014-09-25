#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <algorithm>
#include <functional>

#include "StoreNet.h"
#include "Node.h"

#include "Utilities.h"

using namespace std;

//#define DEBUG

static int gcd(const boost::numeric::ublas::vector<int> &V)
{
	bool Init = false;
	int Res = 1;

	for (unsigned int i = 0; i < V.size(); ++i)
	{
		if (!Init)
		{
			if (V[i] != 0)
			{
				if (V[i] > 0)
					Res = V[i];
				else
					Res = -1*V[i];

				Init = true;
			}
		}
		else
			if (V[i] != 0)
			{
				if (V[i] > 0)
					Res = gcd(Res, V[i]);
				else
					Res = gcd(Res, -1*V[i]);
			}
	}

	return Res;
}

/*! \brief Interne au calcul de flots.

	Utilisé dans des \a foreach pour initialiser le champ \a Info d'une catégorie de noeuds
	(places ou transitions). */

class SetNodeInfo
{
private:
	int i;
public:
	SetNodeInfo(void) {i = 0;};
	void operator()(CamiProdRead::Node *N) {N->SetInfo(i++);};
};

/*! \brief Interne au calcul de flots. 

	Utilisé dans la classe SetIncidence pour initialiser la matrice d'incidence. */

class SetIncidenceAux
{
private:
	boost::numeric::ublas::vector<int> *incidence;
	const int Place;
public:
	SetIncidenceAux(boost::numeric::ublas::vector<int> *i, int p): incidence(i), Place(p) {};
	void operator() (const CamiProdRead::ArcsDescription& A)
	{
		incidence[Place][A.GetNode()->GetInfo()] = A.GetIn() - A.GetOut();
	}
};

/*! \brief Interne au calcul de flots.

	Utilisé pour initialiser la matrice d'incidence. */

class SetIncidence
{
private:
	boost::numeric::ublas::vector<int> *incidence;
	unsigned int NbTr;
public:
	SetIncidence(boost::numeric::ublas::vector<int> *i, unsigned int n): incidence(i), NbTr(n)
	{};
	void operator() (CamiProdRead::Node *P)
	{
		for_each(P->EnumerateArcs.begin(), P->EnumerateArcs.end(), SetIncidenceAux(incidence, 
																					P->GetInfo()));
		
		incidence[P->GetInfo()][NbTr+P->GetInfo()] = 1;
	};
};

/*! \brief Interne au calcul de flots.

	Utilisé dans un \a find_if comme test d'arrêt pour rechercher un élément non nul. */

class VecNotZero
{
private:
	const int col;
public:
	VecNotZero(int c): col(c) {};
	bool operator()(const boost::numeric::ublas::vector<int> &V)
	{return V[col] != 0;};
};

/*! \brief Interne au calcul de flots.

	Utilisé dans un \a foreach pour additionner une ligne avec d"autres lignes. */

class SumRows 
{
private:
	const boost::numeric::ublas::vector<int> &V;
	const int Col;
public:
	SumRows(boost::numeric::ublas::vector<int> &v, int c): V(v), Col(c) 
	{
	};
	void operator() (boost::numeric::ublas::vector<int> &R)
	{
		int RCoeff = -1*V[Col];
		const int VCoeff = R[Col];

		if (VCoeff)
		{
			boost::numeric::ublas::vector<int> TmpV = V;

			if (RCoeff != 1)
				R *= RCoeff;

			if (VCoeff != 1)
				TmpV *= VCoeff;

			R += TmpV;

			int g = gcd(R);

			if (g > 1)
				R /= g;
		}
	};
};

static void PrintCoeff(int Val, bool &Init)
{
	int Coeff;

	if (Val > 0)
	{
		if (!Init)
			cout << " + ";
		else
			Init = false;

		Coeff = Val;
	}
	else
	{
		if (!Init)
			cout << " - ";
		else
		{
			cout << "-";
			Init = false;
		}

		Coeff = -1*Val;
	}

	if (Coeff != 1)
		cout << Coeff << "*";
}

/*!	\brief Affichage d'une ligne de la matrice d'incidence
	\param incidence La matrice d'incidence
	\param j Le numéro de la ligne à afficher
	\param N La description du réseau de Petri
*/

static void PrintLineEq(boost::numeric::ublas::vector<int> incidence[], unsigned int j, 
						StoreNet &N)
{
	const unsigned int Max = N.NbPlaces() + N.NbTransitions();

	StoreNet::Elements::const_iterator It = N.Places.begin();
	bool Init = true;
	int MarqInit = 0;

	for (unsigned int k = N.NbTransitions(); k < Max; ++k)
	{
		if (incidence[j][k])
		{
			MarqInit += incidence[j][k]*((CamiProdRead::Place *)(*It))->GetInitial();
			PrintCoeff(incidence[j][k], Init);
			cout << (*It)->GetName();
		}

		++It;
	}

	cout << " = " << MarqInit << endl;
}

static int FlotsCalc(StoreNet &N)
{
	boost::numeric::ublas::vector<int> incidence[N.NbPlaces()];

// On indique à chaque vecteur le nombre d'éléments qu'il va contenir et mise à zéro de 
// la matrice.

	for (unsigned int i = 0; i < N.NbPlaces(); ++i)
	{
		incidence[i].resize(N.NbPlaces() + N.NbTransitions(), false);
		
		fill(incidence[i].begin(), incidence[i].end(), 0);
	}

// Chaque transition et chaque place mémorise sa place
	
	for_each(N.Transitions.begin(), N.Transitions.end(), SetNodeInfo());

	for_each(N.Places.begin(), N.Places.end(), SetNodeInfo());
	
// Initialisation de la matrice d'incidence

	for_each(N.Places.begin(), N.Places.end(), SetIncidence(incidence, N.NbTransitions()));

// On effectue le calcul

	unsigned int col = 0; // La colonne qu'on cherche à annuler
	unsigned int i;
			
#ifdef DEBUG
	cout << "Avant le calcul" << endl;
	cout << "nbplaces : " << N.NbPlaces() << " nbtrans : " << N.NbTransitions() << endl;

	for (unsigned int i1 = 0; i1 < N.NbPlaces(); ++i1)
		cout << incidence[i1] << endl;
#endif

	for (i = 0; (col < N.NbTransitions()) && (i < N.NbPlaces()); ++i)
	{
		boost::numeric::ublas::vector<int, boost::numeric::ublas::unbounded_array<int, 
												allocator<int> > >* row = incidence + i, *row1;
		// On cherche le pivot

		while ((i < N.NbTransitions()) && (col < N.NbTransitions()) && (incidence[i][col] == 0))
		{
			row1 = find_if(row, incidence+N.NbPlaces(), VecNotZero(col));

			if (row1 < incidence+N.NbPlaces())
				incidence[i].swap(*row1);
			else
				++col;
		}

#ifdef DEBUG
		cout << "Après le pivot" << endl;

		for (unsigned int i1 = 0; i1 < N.NbPlaces(); ++i1)
			cout << incidence[i1] << endl;

		cout << " i = " << i << " col = " << col << endl;

		cout << "nbplaces : " << N.NbPlaces() << " nbtrans : " << N.NbTransitions();
		cout << endl;
#endif

		if (col < N.NbTransitions())
		{	
		// On met à zéro le reste de la colonne
		
		for_each(row+1, incidence+N.NbPlaces(), SumRows(incidence[i], col));

#ifdef DEBUG
		cout << "Après la mise à zéro" << endl;

		for (unsigned int i1 = 0; i1 < N.NbPlaces(); ++i1)
			cout << incidence[i1] << endl;

		cout << "nbplaces : " << N.NbPlaces() << " nbtrans : " << N.NbTransitions() << endl;
#endif
		}
		else
			--i;

		++col;
	}

// On inverse le sens si tous les coefficients sont négatifs

	for(unsigned int j = i; j < N.NbPlaces(); ++j)
	{
		boost::numeric::ublas::vector<int, boost::numeric::ublas::unbounded_array<int, 
													allocator<int> > >::const_iterator row1;

		row1 = find_if(incidence[j].begin(), incidence[j].end(), bind2nd(greater<int>(), 0));

		if (row1 == incidence[j].end())
			incidence[j] *= -1;
	}
		

// Affichage

	for (unsigned int j = i; j < N.NbPlaces(); ++j)
		PrintLineEq(incidence, j, N);

#ifdef DEBUG
	for (unsigned int i1 = 0; i1 < N.NbPlaces(); ++i1)
		cout << incidence[i1] << endl;

	cout << "nbplaces : " << N.NbPlaces() << " nbtrans : " << N.NbTransitions();
	cout << " i : " << i << endl;
#endif

	return 0;
}