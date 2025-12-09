#include<cryptominisat.h>

#include "corona.h"

using namespace std;
using namespace CMSat;

int main(){
  setup_hex_map();
  int n, num_shapes, coronas;
  string s;
  vector<string> shapes;

  // input
  cin >> n;
  cout << n << " coronas\n";
  for (int i = 0; i < n; ++i){
    cin >> num_shapes;
    cout << num_shapes << " ";
    shapes.clear();
    // get the shapes
    for (int j = 0; j < num_shapes; ++j){
      cin >> s;
      cout << s << " ";
      shapes.push_back(s);
    }
    cout << "\n";
    cin >> coronas;
    string s = corona(shapes, coronas);
  }
}

