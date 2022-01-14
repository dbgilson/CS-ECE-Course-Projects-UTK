#include <string>
#include <list>
#include <vector>
using namespace std;

class candyCrush {
public:
  candyCrush(string inputFile);
  int getRowLength();
  int getScore();
  void printCandy();
  int play(int choice);		
protected:
  list<string> candy;
  vector<string> flavors;
  vector<int> points;
  vector<int> probabilities;
  int score;
  int rowLength;
};
