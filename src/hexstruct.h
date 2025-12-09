#include<bits/stdc++.h>
using namespace std;
struct hexStructure{
  int num_hex;
  int num_equiv;
  vector<pair<int, int>> hex_required;
  vector<pair< pair<int, int>, pair<int,int> > > equiv;
  hexStructure(int _num_hex, int _num_equiv, vector<pair<int,int>> vp, 
      vector<pair< pair<int, int>, pair<int,int> > > ep):
      num_hex(_num_hex), num_equiv(_num_equiv), hex_required(vp), equiv(ep)
  {}
};
