#include<string>
#include<vector>

using namespace std;
using namespace CMSat;

const int MAX_CORONAS = 10;
const int TIMEOUT = 20;

// ul, l, dl, dr, r, ur,
const int DIRECTIONS = 6;
const int dx[DIRECTIONS] = {-1, -1, 0, 1, 1, 0};
const int dy[DIRECTIONS] = {1, 0, -1, -1, 0, 1};
string bal_hex[] = {"++-0-0", "+--0+0", "+-+0-0", "+0-000", "+-+-+-", "++-00-", "+-0+0-", "+++---", "++-+--", "+00-00", "+-0-+0", "+--+00", "000000", "+-+-00", "++--00", "+-0+-0", "++0--0", "+-0000"};
vector<string> unbal_hex = {"+-----", "+++++0", "-00000", "++----", "+++-0-", "++-+00", "+++--0", "++-+-0", "++-+0-", "++0++0", "++0000", "+-00+0", "+++000", "++-++-", "+0+000", "+++-+0", "+-+0+0", "-0-0-0", "++-0+0", "+-0--0", "++++-0", "+-+---", "---0-0", "+-+-0-", "+-0+00", "+++0+0", "+++-00", "--0--0", "+-+--0", "+-00-0", "++++++", "-0-000", "+-000-", "+++++-", "++++--", "+---+0", "-00-00", "+---0-", "+++-+-", "+---00", "+--00-", "++++00", "+--0--", "--0000", "+00+00", "-----0", "--0-00", "+++0-0", "---000", "+0-0-0", "++--+0", "+0+0-0", "+-0-0-", "+-+-+0", "------", "++-000", "+--000", "----00", "++0+-0", "++---0", "+--+--", "+0--00", "+-+000", "+--0-0", "+0---0", "+0+0+0", "+--+-0", "+-0-00", "++--0-", "+----0", "++0+00", "+00000", "++0-00", "++-++0"};
vector<string> pos_unbal_hex = {"+0+0-0", "++--+0", "+00+00", "++0++0", "+-0+00", "+++-+-", "+++0+0", "++0-00", "++++++", "++-+-0", "++++00", "++-0+0", "++0+-0", "++0+00", "++0000", "++-000", "+++++0", "++-++-", "+0+0+0", "++-+00", "+++-0-", "++++-0", "+++000", "+-+0+0", "+-+000", "+0+000", "+-+-+0", "+-00+0", "+++-+0", "+++0-0", "++-++0", "++++--", "++-+0-", "+++-00", "+00000", "+++--0", "+++++-"};

int hexagonal_numbers(int x){
  x += 1;
  return 1 + 6 * ((x * (x-1))/2);
}

string transform(string shape, int rot=0, bool ref=false){
  if (ref){
    reverse(shape.begin(), shape.end());
  }
  for (int i = 0; i < rot; ++i){
    char fs = shape[0];
    shape.erase(0,1);
    shape += fs;
  }
  return shape;
}

vector<string> all_s(vector<string> ss, bool disable_ref=false){
  vector<string> shapes;
  for (string s: ss){
    bool ref = false;
    for (int j = 0; j < 2; ++j){
      for (int i=0;i<6;++i){
        string new_s = transform(s, i, ref);
        auto it = find(shapes.begin(), shapes.end(), new_s);
        if (it == shapes.end()) shapes.push_back(new_s);
      }
      if (!disable_ref){
        ref = true;
      } else {
        break;
      }
    }
  }
  return shapes;
}

int parity(const string &s){
  int pol = 0;
  for (char c: s){
    if (c == '+') pol++;
    if (c == '-') pol--;
  }
  return pol;
}

string invert(const string &s){
  string ret;
  for (char c: s){
    if (c == '+') ret = ret + '-';
    else if (c == '-') ret = ret + '+';
    else ret = ret + '0';
  }
  return ret;
}


