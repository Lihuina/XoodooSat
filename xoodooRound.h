#ifndef XOODOOROUND_H
#define XOODOOROUND_H

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include "gen_xoodoo_Chi_cnf.h"
#include "gen_xoodoo_AS_cnf.h"

namespace XOODOOSAT {

typedef unsigned int tXoodooLane;
typedef vector<tXoodooLane> tXoodooState;
typedef vector<unsigned int> Bitmap;//for Bitmap A, A[i] = 0 or A[i] = 1
typedef vector<unsigned int> State;//only contains the bit indexs that is set 1, i.e. {122,306}
typedef vector<State> States;

#define X 4
#define Y 3
#define Z 32 //sizeof(tXoodooLane)*8
#define var_num X*Y*Z //the number of variables(bits) of a state

#define default_obj_file_path "./" //path to AS cnf

#define indexXZ(x, z) ( Z*((x)%X) + ((z)%Z) ) /* 32*x + z */
#define indexXY(x, y) ( ((x)%X) + X*((y)%Y) ) /* x + 4*y */
#define ROLxoo(a, offset) ((offset != 0) ? ((((tXoodooLane)a) << offset) ^ (((tXoodooLane)a) >> (sizeof(tXoodooLane)*8-offset))) : a)

/*
 3 round: a0 -> b0 -> a1 -> b1 -> a2 -> b2 -> a3, bi=lambda(ai), ai+1=chi(bi)
 3 round trail core: a1 -> b1 -> a2 -> b2
 state to sum weight: a1, a2, b2 (a1, a2,..., an-1, bn-1)
*/

class XoodooRound
{
/*
core_state_num: how many states in the trail core
AS_weight_num: trail weight to bound
AS_state_num: how many states in the trail core to sum weight; in the example above, w(a1)+w(a2)+w(b2), 3 states to sum weight
round_num: how many rounds to analysis
AS_var_num: the number of variables(bits) of all AS nodes
AS_node_var_num: the number of variables(bits) of a AS node
AS_mode: weight sum mode, 0 for atmost, 1 for atleast, 2 for equals.
round_var_num: the number of variables(bits) of a round
*/
public:
    int core_state_num, AS_weight_num, AS_state_num, round_num;
    int AS_var_num, AS_node_var_num, AS_mode, round_var_num;
    int rho_plane[2], theta_L1[2], theta_L2[2], rhoW_L1[2], rhoW_L2[2], rhoE_L1[2], rhoE_L2[2];//parameters in the theta and rho process
	string objFilePath;//path to save the AS cnf file
    int thread_num;
    SATSolver solver;//the solver

    XoodooRound(int round, int AS_max, int thread, int mode);//constructor
    //function in a Xoodoo round
    void theta(tXoodooState &A);//A is a state, below is the same
    void rhoW(tXoodooState &A);
    void rhoE(tXoodooState &A);
    void chi(tXoodooState &A);
    tXoodooState lambda(tXoodooState A);
    State extendChi(State Bi);

    void Bit2XooState(const Bitmap &A, tXoodooState &B);
    void XooState2Bit(const tXoodooState &A, Bitmap &B);
    void Bit2State(const Bitmap &A, State &B);
    void State2Bit(const State &A, Bitmap &B);
    void Bit2Plane(const Bitmap &A, vector<tXoodooLane> &B);//A,B are planes in different format
    void Plane2Bit(const vector<tXoodooLane> &A, Bitmap &B);
    void State2XooState(const State &A, tXoodooState &B);
    void XooState2State(const tXoodooState &A, State &B);

    int caculateXooStateWeight(const tXoodooState &A);

    State ShiftXZ(const vector<int> &dx_dz, const State &A);//left shift (dx,dz)
    bool StateEqualAfterShift(const State &A, const State &B);//whether A could shiftXZ to B
    bool isSmaller(const State &A, const State &B);//compare 2 state
    vector<int> genSmallestState(const State &A);

    void display(ostream& fout, const State &A);//display state
    void displayXooState(ostream& fout, const tXoodooState &A);//display tXoodooState

    void gen_RhoW_T(map<unsigned int, unsigned int>& RhoW_index);//generate a map for rhow(state)
    void gen_RhoE_T(map<unsigned int, unsigned int>& RhoE_index, map<unsigned int, unsigned int>& inverse_RhoE_index);//generate maps for rhoe(state) and rhoe-1(state)
    void gen_Theta_T(vector<vector<unsigned int>>& relation);//generate a map for theta(state)
    void gen_obj_T(vector<vector<int>> &obj);//generate cnf for AS
    void gen_extend_AS_cnf_num(string &cnf_num);//get var list for pysat.card function

    void ban_solution(const map<State,int> &A);//ban found and shifted solutions
    int get_weight();//return the trail weight by caculating AS
    
    void write_result(const string pathname, const int weight, const int solution_count, const States &solution);//writing solution to a file in a readable format
    void solve_and_output(const string pathname);//solve the phase and output the result
    void read2Vector(vector<vector<int>>& res, const string pathname);//read cnf generated by python-sat
    bool read2States(States& res, ifstream &infile, int mode);//read a result state in a readable file, mode 0 for file generated by this programme, 1 for DC file generated by previous work
    void check_trails(const string pathname, int i, int mode);//check and print the i th trail results in a readable file
    vector<vector<int>> compare_trails(const string path_res, const string path_DC);//compare trail results with previous XOODOO work, returns the different trail result number

    void XoodooRound_AS();//differential analysis round function, using trail extension
};


}//end namespace

#endif