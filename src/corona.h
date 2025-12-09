#include<cryptominisat.h>
#include<utility>
#include<map>
#include<iostream>

#include "hex.h"

using namespace std;
using namespace CMSat;

map<pair<int, int>, int> coords_to_idx;
map<int, pair<int, int>> idx_to_coords;

string corona(vector<string> shapes, int num, bool strict = false){
  vector<string> all_shapes;
  all_shapes = all_s(shapes);
  shapes = all_shapes;
  int n = all_shapes.size();
  int hex_num = hexagonal_numbers(num);
  int var_size = (n+12)*hex_num;

  SATSolver solver;
  vector<Lit> clause;

  solver.new_vars(var_size);

  if (strict){
    clause.push_back(Lit(0, false));
    solver.add_clause(clause);
  }

  // for each hex in grid
  for (int i = 0; i < hex_num; ++i){
    clause.clear();
    // at least one of the shapes must be in that place
    for (int j=0; j < n; ++j){
      clause.push_back(Lit(i*(n+12) + j, false));
    }
    solver.add_clause(clause);
    //exactly 1 of the shapes is in that place
    for (int j=0; j < n; ++j){
      for (int k=j+1; k < n; ++k){
        clause.clear();
        clause.push_back(Lit(i*(n+12) + j, true));
        clause.push_back(Lit(i*(n+12) + k, true));
        solver.add_clause(clause);
      }
    }
    // 01, 23, 45, etc, each have 0 true: nick, 1 true: line, both true: bump
    for (int j=0; j < n; ++j){
      for (int k=0; k < 6; ++k){
        if (shapes[j][k] == '-'){
          clause.clear();
          clause.push_back(Lit(i*(n+12) + j, true));
          clause.push_back(Lit(i*(n+12) + n + 2*k, true));
          solver.add_clause(clause);
          clause.clear();
          clause.push_back(Lit(i*(n+12) + j, true));
          clause.push_back(Lit(i*(n+12) + n + 2*k+1, true));
          solver.add_clause(clause);
        } else if (shapes[j][k] == '0'){
          clause.clear();
          clause.push_back(Lit(i*(n+12) + j, true));
          clause.push_back(Lit(i*(n+12) + n + 2*k, false));
          solver.add_clause(clause);
          clause.clear();
          clause.push_back(Lit(i*(n+12) + j, true));
          clause.push_back(Lit(i*(n+12) + n + 2*k+1, true));
          solver.add_clause(clause);
        } else if (shapes[j][k] == '+'){
          clause.clear();
          clause.push_back(Lit(i*(n+12) + j, true));
          clause.push_back(Lit(i*(n+12) + n + 2*k, false));
          solver.add_clause(clause);
          clause.clear();
          clause.push_back(Lit(i*(n+12) + j, true));
          clause.push_back(Lit(i*(n+12) + n + 2*k+1, false));
          solver.add_clause(clause);
        }
      }
    }
    // for each direction, make sure bump goes to nick, or both lines
    for (int j=0; j < 6; ++j){
      int k = (j + 3) % 6;
      pair<int,int> co = make_pair(idx_to_coords[i].first+dx[j], idx_to_coords[i].second+dy[j]);
      int pair_id = coords_to_idx[co];
      if (pair_id >= hex_num) continue;
      // exact 1, not both 1
      clause.clear();
      clause.push_back(Lit(i*(n+12) + n + 2*j, false));
      clause.push_back(Lit(pair_id*(n+12) + n + 2*k+1, false));
      solver.add_clause(clause);
      clause.clear();
      clause.push_back(Lit(i*(n+12) + n + 2*j, true));
      clause.push_back(Lit(pair_id*(n+12) + n + 2*k+1, true));
      solver.add_clause(clause);
      // exact 1, not both 2
      clause.clear();
      clause.push_back(Lit(i*(n+12) + n + 2*j+1, false));
      clause.push_back(Lit(pair_id*(n+12) + n + 2*k, false));
      solver.add_clause(clause);
      clause.clear();
      clause.push_back(Lit(i*(n+12) + n + 2*j+1, true));
      clause.push_back(Lit(pair_id*(n+12) + n + 2*k, true));
      solver.add_clause(clause);
    }
  }
  lbool ret = solver.solve();
  if (ret == l_True) {
    // printing info
    cout << hex_num << "\n";
    for (int i = 0; i < hex_num; ++i){
      cout << idx_to_coords[i].first << " " << idx_to_coords[i].second << " ";
      for (int j = 0; j < n; ++j){
        if (solver.get_model()[i*(n+12)+j] == l_True) cout << transform(shapes[j], 0, true);
      }
      cout << "\n";
    }
    // end of printing info
    return "Found";
  }
  cout << 0 << "\n";
  return "Not Found";
}

void setup_hex_map(){
  int x = 0;
  int y = 0;
  int idx = 0;
  int RADIUS = 100;
  for (int cur_rad = 0; cur_rad <= RADIUS; ++cur_rad){
    coords_to_idx[make_pair(x, y)] = idx;
    idx_to_coords[idx] = make_pair(x, y);
    idx++;
    for (int d = 0; d < DIRECTIONS; ++d){
      for (int j = 0; j < cur_rad; ++j){
        x += dx[d];
        y += dy[d];
        if (d != DIRECTIONS - 1 || j != cur_rad-1) {
          coords_to_idx[make_pair(x, y)] = idx;
          idx_to_coords[idx] = make_pair(x, y);
          idx++;
        }
      }
    }
    x += 1;
  }
}

