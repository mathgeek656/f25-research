#include<cryptominisat.h>
#include "hex.h"
#include "tiles41.h"

double timeout_limit = 0.1;

using namespace std;

pair<int, int> get_pair_idx(int idx, const hexStructure& hs){
  if (idx < hs.num_hex) return hs.hex_required[idx];
  else return hs.equiv[idx-hs.num_hex].first;
}

int get_grid_idx(pair<int, int> pi, const hexStructure& hs){
  for (int i = 0; i < hs.num_hex; ++i){
    if (pi == hs.hex_required[i]) return i;
  }
  for (int i = 0; i < hs.num_equiv; ++i){
    if (pi == hs.equiv[i].first) return i + hs.num_hex;
  }
  return -1;
}

string periodic_tiling(vector<string> shapes, const hexStructure& hs){
  int grid_size = hs.num_hex + hs.num_equiv;
  vector<string> all_shapes;
  all_shapes = all_s(shapes);
  shapes = all_shapes;
  int n = all_shapes.size();
  int var_size = (n + 12) * grid_size;

  SATSolver solver;
  vector<Lit> clause;
  solver.new_vars(var_size);
  
  // for each hex in grid
  for (int i = 0; i < grid_size; ++i){
    clause.clear();
    for (int j = 0; j < n; ++j){
      clause.push_back(Lit(i*(n+12) + j, false));
    }
    solver.add_clause(clause);
    // exactly 1 of the shapes is in place
    for (int j = 0; j < n; ++j){
      for (int k = j+1; k < n; ++k){
        clause.clear();
        clause.push_back(Lit(i*(n+12) + j, true));
        clause.push_back(Lit(i*(n+12) + k, true));
        solver.add_clause(clause);
      }
    }
    // determine nick, line, bump
    for (int j = 0; j < n; ++j){
      for (int k = 0; k < 6; ++k){
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
    // nick to bump, etc
    for (int j = 0; j < 6; ++j){
      int k = (j + 3) % 6;
      pair<int, int> co = make_pair(get_pair_idx(i, hs).first + dx[j], get_pair_idx(i, hs).second + dy[j]);
      int pair_id = get_grid_idx(co, hs);
      if (pair_id == -1) continue;
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
    // equiv conditions
    for (int j = 0; j < hs.num_equiv; ++j){
      int match1 = j + hs.num_hex;
      int match2 = get_grid_idx(hs.equiv[j].second, hs);
      for (int k = 0; k < n; ++k){
        // both same shape not that shape
        clause.clear();
        clause.push_back(Lit(match1*(n+12) + k, false));
        clause.push_back(Lit(match2*(n+12) + k, true));
        solver.add_clause(clause);
        clause.clear();
        clause.push_back(Lit(match1*(n+12) + k, true));
        clause.push_back(Lit(match2*(n+12) + k, false));
        solver.add_clause(clause);
      }
    }
  }
  solver.set_timeout_all_calls(timeout_limit);
  lbool ret = solver.solve();
  if (ret == l_True){
    cout << hs.num_hex << "\n";
    for (int i = 0; i < hs.num_hex; ++i){
      cout << get_pair_idx(i,hs).first << " " << get_pair_idx(i,hs).second << " ";
      for (int j = 0; j < n; ++j){
        if (solver.get_model()[i*(n+12)+j] == l_True) cout << transform(shapes[j], 0, true);
      }
      cout << "\n";
    }
    return "Found";
  } else {
    return "Not found";
  }
}

string all_iso(vector<string> vs){
  for (unsigned int i = 0; i < hexes.size(); ++i){
    if (periodic_tiling(vs, hexes[i]) == "Found") {
      return "Found";
    }
  }
  cout << 0 << "\n";
  return "Not found";
}

int main(){
  int num_pages, num_shapes;
  string s;
  cin >> num_pages;
  cout << num_pages << " tt" << "\n";
  for (int i = 0; i < num_pages; ++i){
    cin >> num_shapes;
    cout << num_shapes << " ";
    vector<string> shapes;
    for (int j = 0; j < num_shapes; ++j){
      cin >> s;
      cout << s << " ";
      shapes.push_back(s);
    }
    cout << "\n";
    s = all_iso(shapes);
  }

}
