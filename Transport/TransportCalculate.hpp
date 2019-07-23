#pragma once

#include "stdafx.h"

//template<typename TransportData>;
typedef double TransportData;
/*
enum RetCode {
	TranspError,
	TranspSucc,
	TranspFail,
};
*/
// Transport calculate
namespace TC
{
	struct int2
	{
		UINT x, y;
	};

	bool NorthWest(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData *tResult);

	// {A, B, size_a, size_b, C} - входные данные
	// {bool, tResult} - выходные данные
	bool MinimalRate(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData *C, TransportData *tResult);

	// Память под U, V не выделяется в функции
	bool CalculateUVPotentials(UINT size_a, UINT size_b, TransportData *T, TransportData *C, TransportData *U, TransportData *V);

	void CalcHorizontalPotential(UINT iU, UINT jV, UINT &size_a, UINT &size_b, TransportData *T, TransportData *C, TransportData *U, TransportData *V);
	void CalcVerticalPotential(UINT iU, UINT jV, UINT &size_a, UINT &size_b, TransportData *T, TransportData *C, TransportData *U, TransportData *V);

	bool CheckOptimal(UINT size_a, UINT size_b, TransportData *C, TransportData *U, TransportData *V, int2 *coord);
	void Optimize(UINT size_a, UINT size_b, TransportData *T, TransportData *C, int2 *coord);

	bool SeriesHorizontal(UINT i0, UINT j0, UINT &size_a, UINT &size_b, TransportData *T, std::vector<int2> *Coords, int2 *coord);
	bool SeriesVertical(UINT i0, UINT j0, UINT &size_a, UINT &size_b, TransportData *T, std::vector<int2> *Coords, int2 *coord);

	bool TransportCheck(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData *T);
	int  BalanceCheck(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData &Difference);
	TransportData ShipCost(TransportData *T, TransportData *C, UINT size_a, UINT size_b);
	bool Calculate(UINT size_a, UINT size_b, TransportData *T, TransportData *C);
};