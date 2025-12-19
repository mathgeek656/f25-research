#include<cryptominisat.h>
#include "hex.h"
#include "tiles41.h"

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

string periodic_tiling(vector<string> shapes, const hexStructure& hs, int marks=1, bool disable_refs=false){
  int grid_size = hs.num_hex + hs.num_equiv;
  vector<string> all_shapes;
  all_shapes = all_s(shapes, disable_refs);
  shapes = all_shapes;
  int n = all_shapes.size();
  int extra_vars = 6+6*marks;
  int var_size = (n + extra_vars) * grid_size;

  SATSolver solver;
  vector<Lit> clause;
  solver.new_vars(var_size);
  
  // for each hex in grid
  for (int i = 0; i < grid_size; ++i){
    clause.clear();
    for (int j = 0; j < n; ++j){
      clause.push_back(Lit(i*(n+extra_vars) + j, false));
    }
    solver.add_clause(clause);
    // exactly 1 of the shapes is in place
    for (int j = 0; j < n; ++j){
      for (int k = j+1; k < n; ++k){
        clause.clear();
        clause.push_back(Lit(i*(n+extra_vars) + j, true));
        clause.push_back(Lit(i*(n+extra_vars) + k, true));
        solver.add_clause(clause);
      }
    }
    // determine nick, line, bump
    for (int j = 0; j < n; ++j){
      for (int k = 0; k < 6; ++k){
        for (int p = 1; p < marks; ++p){
          bool is_mark = shapes[j][k] == 'a'+p || shapes[j][k] == 'A'+p;
          clause.clear();
          clause.push_back(Lit(i*(n+extra_vars) + j, true));
          clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*k+p+1, is_mark));
          solver.add_clause(clause);
        }
        if (shapes[j][k] >= 'a' && shapes[j][k] <= 'z'){
          clause.clear();
          clause.push_back(Lit(i*(n+extra_vars) + j, true));
          clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*k, true));
          solver.add_clause(clause);
          clause.clear();
          clause.push_back(Lit(i*(n+extra_vars) + j, true));
          clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*k+1, true));
          solver.add_clause(clause);
        } else if (shapes[j][k] == '0'){
          clause.clear();
          clause.push_back(Lit(i*(n+extra_vars) + j, true));
          clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*k, false));
          solver.add_clause(clause);
          clause.clear();
          clause.push_back(Lit(i*(n+extra_vars) + j, true));
          clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*k+1, true));
          solver.add_clause(clause);
        } else if (shapes[j][k] >= 'A' && shapes[j][k] <= 'Z'){
          clause.clear();
          clause.push_back(Lit(i*(n+extra_vars) + j, true));
          clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*k, false));
          solver.add_clause(clause);
          clause.clear();
          clause.push_back(Lit(i*(n+extra_vars) + j, true));
          clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*k+1, false));
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
      for (int p = 1; p < marks; ++p){
        clause.clear();
        clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*j+p+1, false));
        clause.push_back(Lit(pair_id*(n+extra_vars) + n + (marks+1)*k+p+1, true));
        solver.add_clause(clause);

        clause.clear();
        clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*j+p+1, true));
        clause.push_back(Lit(pair_id*(n+extra_vars) + n + (marks+1)*k+p+1, false));
        solver.add_clause(clause);

      }
      // exact 1, not both 1
      clause.clear();
      clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*j, false));
      clause.push_back(Lit(pair_id*(n+extra_vars) + n + (marks+1)*k+1, false));
      solver.add_clause(clause);
      clause.clear();
      clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*j, true));
      clause.push_back(Lit(pair_id*(n+extra_vars) + n + (marks+1)*k+1, true));
      solver.add_clause(clause);
      // exact 1, not both 2
      clause.clear();
      clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*j+1, false));
      clause.push_back(Lit(pair_id*(n+extra_vars) + n + (marks+1)*k, false));
      solver.add_clause(clause);
      clause.clear();
      clause.push_back(Lit(i*(n+extra_vars) + n + (marks+1)*j+1, true));
      clause.push_back(Lit(pair_id*(n+extra_vars) + n + (marks+1)*k, true));
      solver.add_clause(clause);
    }
    // equiv conditions
    for (int j = 0; j < hs.num_equiv; ++j){
      int match1 = j + hs.num_hex;
      int match2 = get_grid_idx(hs.equiv[j].second, hs);
      for (int k = 0; k < n; ++k){
        // both same shape not that shape
        clause.clear();
        clause.push_back(Lit(match1*(n+extra_vars) + k, false));
        clause.push_back(Lit(match2*(n+extra_vars) + k, true));
        solver.add_clause(clause);
        clause.clear();
        clause.push_back(Lit(match1*(n+extra_vars) + k, true));
        clause.push_back(Lit(match2*(n+extra_vars) + k, false));
        solver.add_clause(clause);
      }
    }
  }
  lbool ret = solver.solve();
  if (ret == l_True){
    cout << hs.num_hex << "\n";
    for (int i = 0; i < hs.num_hex; ++i){
      cout << get_pair_idx(i,hs).first << " " << get_pair_idx(i,hs).second << " ";
      for (int j = 0; j < n; ++j){
        if (solver.get_model()[i*(n+extra_vars)+j] == l_True) cout << transform(shapes[j], 0, true);
      }
      cout << "\n";
    }
    return "Found";
  } else {
    return "Not found";
  }
}

string all_iso(vector<string> vs, int num_marks = 1, bool disable_refs = false){
  for (unsigned int i = 0; i < hexes.size(); ++i){
    if (periodic_tiling(vs, hexes[i], num_marks, disable_refs) == "Found") {
      return "Found";
    }
  }
  cout << 0 << "\n";
  return "Not found";
}

int main(){
  int num_pages, num_shapes, num_marks;
  string s;
  cin >> num_pages;
  cout << num_pages << " tt" << "\n";
  for (int i = 0; i < num_pages; ++i){
    cin >> num_shapes;
    cout << num_shapes << " ";
    cin >> num_marks;
    vector<string> shapes;
    for (int j = 0; j < num_shapes; ++j){
      cin >> s;
      cout << s << " ";
      shapes.push_back(s);
    }
    cout << "\n";
    s = all_iso(shapes, num_marks, true);
    cout << "\n\n"; //new page
  }

}
