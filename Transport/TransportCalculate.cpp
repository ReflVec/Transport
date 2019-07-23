#include "TransportCalculate.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------//
//--------------------------- ������������� ������� ������-��������� ���� ---------------------------------//
//---------------------------------------------------------------------------------------------------------//
bool TC::NorthWest(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData *tResult)
{
	TransportData t, *A1, *B1; // ��������� ���������, ������� ��������, ������� ������������

	if (A == nullptr ||
		B == nullptr ||
		tResult == nullptr ||
		size_a == 0 ||
		size_b == 0) {
		return false;
	} // �������� �� ������� ��������� � ������� ������

	memset(tResult, 0, sizeof(TransportData) * size_a * size_b); // ������������� � 0 ��� �������� �������

	A1 = new TransportData[size_a];
	B1 = new TransportData[size_b];

	memcpy(A1, A, sizeof(TransportData)*size_a);// �������� ��� �������� �� ������� A � ������ A1
	memcpy(B1, B, sizeof(TransportData)*size_b);// �������� ��� �������� �� ������� A � ������ A1

	for (UINT i = 0; i < size_a; ++i)
	{
		for (UINT j = 0; j < size_b; ++j)
		{
			if (A1[i] == 0.0) // ���� �� ������ �� ����� ������ �� ��������, �� ��������� ���� �� ������.
				break;
			else if (A1[i] < 0.0) {
				delete[] A1, B1;
				return false;
			}

			if (B1[j] == 0.0) // ���� �� ������������� ����������� ��� �������� �����������, �� ��� � ����������.
				continue;
			else if (B1[j] < 0.0) {
				delete[] A1, B1;
				return false;
			}

			t = min(A1[i], B1[j]); // ����� ������������

			A1[i] -= t; // �������� ���� �� ���� ��, ���� �� ������ ������������.
			B1[j] -= t; // �������� ��� ��������� ������ ������������ ������, ��� ��� ������� �������������� ������������ ������������.

			tResult[i*size_b + j] = t; // ���������� ��������� � ������ ���� �� ����� ����.
		}
	}

	delete[] A1, B1;

	return true;
}
//---------------------------------------------------------------------------------------------------------//
//---------------------------- ������������� ������� ����������� ������� ----------------------------------//
//---------------------------------------------------------------------------------------------------------//
bool TC::MinimalRate(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData *C, TransportData *tResult)
{
	TransportData t, min_c, *A1, *B1; // ��������� ���������, ����������� �����, ������� ��������, ������� ������������
	int2 coords = { 0, 0 }; // ���������� ������
	bool raspred = true; // ���������� ��� ������ �� ����� ������������� ��������

	if (A == nullptr ||
		B == nullptr ||
		C == nullptr ||
		tResult == nullptr ||
		size_a == 0 ||
		size_b == 0) {
		return false;
	} // �������� �� ������� ��������� � ������� ������

	memset(tResult, 0, sizeof(TransportData) * size_a * size_b); // ������������� � 0 ��� �������� �������

	A1 = new TransportData[size_a];
	B1 = new TransportData[size_b];

	memcpy(A1, A, sizeof(TransportData)*size_a); // �������� ��� �������� �� ������� A � ������ A1
	memcpy(B1, B, sizeof(TransportData)*size_b); // �������� ��� �������� �� ������� A � ������ A1

	// ���������� ����� ����������� �����
	for (UINT z = 0; z < size_a*size_b; ++z)
	{
		raspred = true;
		min_c = std::numeric_limits<TransportData>::infinity();

		for (UINT i = 0; i < size_a; ++i)
		{
			for (UINT j = 0; j < size_b; ++j)
			{
				UINT idx = i*size_b + j;

				if (tResult[idx] > 0) continue; // ���������� ������ ����������� �����, �.�. �������� ��� ���������

				if (A1[i] == 0.0) // ���� �� ������ �� ����� ������ �� ��������, �� ���������� ����.
					break;
				else if (A1[i] < 0.0) {
					delete[] A1, B1;
					return false;
				}

				if (B1[j] == 0.0) // ���� �� ������������� ����������� ��� �������� �����������, �� ��� � ����������.
					continue;
				else if (B1[j] < 0.0) {
					delete[] A1, B1;
					return false;
				}

				raspred = false; // �������� �� ������������

				if (C[idx] < min_c)
				{
					min_c = C[idx];
					// ���������� ���������� ������
					coords.y = i;
					coords.x = j;
				}
			}
		}

		if (raspred) break; // ���� ������������ ��� �������� ������������, �� �������

		t = min(A1[coords.y], B1[coords.x]); // ����� ������������

		A1[coords.y] -= t; // �������� ���� �� ���� ��, ���� �� ������ ������������.
		B1[coords.x] -= t; // �������� ��� ��������� ������ ������������ ������, ��� ��� ������� �������������� ������������ ������������.

		tResult[coords.y*size_b + coords.x] = t; // ���������� ��������� � ������ ���� �� ����� ����.
	}

	delete[] A1, B1;

	return true;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
//----------------------------- ������ ����������� ������������ ������� -----------------------------------//
//---------------------------------------------------------------------------------------------------------//
bool TC::CalculateUVPotentials(UINT size_a, UINT size_b, TransportData *T, TransportData *C, TransportData *U, TransportData *V) // �.�. �����������, �� ����������� � ����������� �������
{
	TransportData dNaN = std::numeric_limits<TransportData>::quiet_NaN();
	//TransportData dInf = numeric_limits<TransportData>::infinity();

	if (T == nullptr ||
		C == nullptr ||
		U == nullptr ||
		V == nullptr ||
		size_a == 0 ||
		size_b == 0) {
		return false;
	} // �������� �� ������� ��������� � ������� ������

	for (UINT i = 0; i < size_a; ++i)
		U[i] = dNaN;

	for (UINT j = 0; j < size_b; ++j)
		V[j] = dNaN;

	U[0] = 0;

	CalcHorizontalPotential(0, 0, size_a, size_b, T, C, U, V);

	for (UINT j = 0; j < size_b; ++j)
		if (isnan(V[j]))
			CalcVerticalPotential(0, j, size_a, size_b, T, C, U, V);

	for (UINT i = 0; i < size_a; ++i)
		if (isnan(U[i]))
			CalcHorizontalPotential(i, 0, size_a, size_b, T, C, U, V);

	return true;
}
//---------------------------------------------------------------------------------------------------------//
//--------------------------------- ������ ����������� �� ����������� -------------------------------------//
//---------------------------------------------------------------------------------------------------------//
void TC::CalcHorizontalPotential(UINT iU, UINT jV, UINT &size_a, UINT &size_b, TransportData *T, TransportData *C, TransportData *U, TransportData *V)
{
	if (isnan(U[iU]))
		return;

	for (UINT j = 0; j < size_b; ++j) {
		UINT idx = iU*size_b + j;

		if (T[idx] == 0.0 || !isnan(V[j])) {
			continue;
		}
		else {
			V[j] = C[idx] - U[iU];
			CalcVerticalPotential(iU, j, size_a, size_b, T, C, U, V);
		}
	}
}
//----------------------------------- ������ ����������� �� ��������� -------------------------------------//
void TC::CalcVerticalPotential(UINT iU, UINT jV, UINT &size_a, UINT &size_b, TransportData *T, TransportData *C, TransportData *U, TransportData *V)
{
	TransportData dInf = std::numeric_limits<TransportData>::infinity();

	if (isnan(V[jV]))
		return;

	for (UINT i = 0; i < size_a; ++i) {
		UINT idx = i*size_b + jV;

		if (T[idx] == 0.0 || !isnan(U[i])) {
			continue;
		}
		else {
			U[i] = C[idx] - V[jV];
			CalcHorizontalPotential(i, jV, size_a, size_b, T, C, U, V);
		}
	}
}
//---------------------------------------------------------------------------------------------------------//
//-------------------------- ������� ��� �������� ������������� ��������� ---------------------------------//
//---------------------------------------------------------------------------------------------------------//

bool TC::CheckOptimal(UINT size_a, UINT size_b, TransportData *C, TransportData *U, TransportData *V, int2 *coord)
{
	bool opt = true;
	*coord = { 0, 0 }; // ���������� ������
	TransportData d = 0, d_min = std::numeric_limits<TransportData>::infinity();

	for (UINT i = 0; i < size_a; ++i)
	{
		for (UINT j = 0; j < size_b; ++j)
		{
			d = C[i*size_b + j] - U[i] - V[j];

			if (d < 0) opt = false;

			if (d < d_min) {
				d_min = d;
				coord->y = i;
				coord->x = j;
			}
		}
	}

	return opt;
}
//---------------------------------------------------------------------------------------------------------//
//------------ ����������������� ��������� ��� ����������� ����� ������������ �������� ����� --------------//
//---------------------------------------------------------------------------------------------------------//
void TC::Optimize(UINT size_a, UINT size_b, TransportData *T, TransportData *C, int2 *coord)
{
	// ����������� �����
	TransportData V = std::numeric_limits<TransportData>::quiet_NaN(), t; // NaN - �� �����
	std::vector<int2> Coords; // ���������� ���������

							  // ��������� ������ � ������� �������� ������
	SeriesHorizontal(coord->y, coord->x, size_a, size_b, T, &Coords, coord);

	// ������� ����������� �������� �� ������������� �����������
	for (size_t p = 1; p < Coords.size(); p += 2) {
		t = T[Coords[p].y*size_b + Coords[p].x];

		if (t < V || isnan(V)) {
			V = t;
		}
	}

	for (size_t p = 0; p < Coords.size(); ++p) {
		if (p & 1) {// �������� �����
			T[Coords[p].y*size_b + Coords[p].x] -= V;
		}
		else {
			T[Coords[p].y*size_b + Coords[p].x] += V;
		}
	}

	return;
}

bool TC::SeriesHorizontal(UINT i0, UINT j0, UINT &size_a, UINT &size_b, TransportData *T, std::vector<int2> *Coords, int2 *coord)
{
	int2 values;

	for (UINT j = 0; j < size_b; ++j)
	{
		if (T[i0*size_b + j] == 0.0 || j == j0)
			continue;

		if (SeriesVertical(i0, j, size_a, size_b, T, Coords, coord)) {
			values.y = i0;
			values.x = j;

			Coords->push_back(values);
			return true;
		}
	}

	return false;
}

bool TC::SeriesVertical(UINT i0, UINT j0, UINT &size_a, UINT &size_b, TransportData *T, std::vector<int2> *Coords, int2 *coord)
{
	int2 values;

	for (UINT i = 0; i < size_a; ++i)
	{
		if (i == coord->y && j0 == coord->x)
		{
			values.y = i;
			values.x = j0;

			Coords->push_back(values);
			return true;
		}

		if (i == i0 || T[i*size_b + j0] == 0.0)
			continue;

		if (SeriesHorizontal(i, j0, size_a, size_b, T, Coords, coord)) {
			values.y = i;
			values.x = j0;

			Coords->push_back(values);
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------//
//----------------------------------- �������� ������������� ������ ---------------------------------------//
//---------------------------------------------------------------------------------------------------------//
bool TC::TransportCheck(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData *T)
{
	TransportData Sum;

	for (UINT i = 0; i < size_a; ++i) {
		Sum = 0;
		for (UINT j = 0; j < size_b; ++j) {
			Sum += T[i*size_b + j];
		}

		if (Sum != A[i])
			return false;
	}

	for (UINT j = 0; j < size_b; ++j) {
		Sum = 0;
		for (UINT i = 0; i < size_a; ++i) {
			Sum += T[i*size_b + j];
		}

		if (Sum != B[j])
			return false;
	}

	return true;
};
//---------------------------------------------------------------------------------------------------------//
//------------------------------------ �������� ������������������ ----------------------------------------//
//---------------------------------------------------------------------------------------------------------//
int TC::BalanceCheck(TransportData *A, TransportData *B, UINT size_a, UINT size_b, TransportData &Difference)
{
	TransportData Sum1 = 0, Sum2 = 0;

	for (UINT i = 0; i < size_a; ++i) {
		Sum1 += A[i];
	}

	for (UINT j = 0; j < size_b; ++j) {
		Sum2 += B[j];
	}

	if (Sum1 == Sum2) {
		Difference = 0;
		return 0; // ���� ��������������
	}
	else if (Sum1 > Sum2) {
		Difference = Sum1 - Sum2; // ���� ����� A[i] > ����� B[j]
		return 1;
	}
	else if (Sum2 > Sum1) {
		Difference = Sum2 - Sum1; // ���� ����� B[j] > ����� A[i]
		return 2;
	}
};
//---------------------------------------------------------------------------------------------------------//
//---------------------------------- ���������� ��������� ��������� ---------------------------------------//
//---------------------------------------------------------------------------------------------------------//
TransportData TC::ShipCost(TransportData *T, TransportData *C, UINT size_a, UINT size_b)
{
	TransportData Sum = 0;

	for (UINT i = 0; i < size_a; ++i)
		for (UINT j = 0; j < size_b; ++j)
			Sum += T[i*size_b + j] * C[i*size_b + j];

	return Sum;
}
//---------------------------------------------------------------------------------------------------------//
//------------------------------------ ������ ������������ ������� ----------------------------------------//
//---------------------------------------------------------------------------------------------------------//
bool TC::Calculate(UINT size_a, UINT size_b, TransportData *T, TransportData *C)
{
	TransportData *U, *V;
	int2 coord;

	if (T == nullptr ||
		C == nullptr ||
		size_a == 0 ||
		size_b == 0) {
		return false;
	} // �������� �� ������� ��������� � ������� ������

	U = new TransportData[size_a];
	V = new TransportData[size_b];

	// ������ �����������
	if (!TC::CalculateUVPotentials(size_a, size_b, T, C, U, V)) {
		MessageBox(nullptr, L"���������� ���������� ����������", L"��������", MB_OK | MB_ICONASTERISK);
		return false;
	}

	// �������� �������������
	if (TC::CheckOptimal(size_a, size_b, C, U, V, &coord))
		return false;

	// ����������� ������������ �������
	TC::Optimize(size_a, size_b, T, C, &coord);

	delete[] U, V;

	return true;
}
//---------------------------------------------------------------------------------------------------------//