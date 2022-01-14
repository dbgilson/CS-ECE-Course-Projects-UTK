//CS302, Lab 5
//By: David Gilson
//The purpose of this code is to take in a list of cities and their cooresponding information, calculate the distance and time from one another, and create a graph-like 
//structure in order to sort find the minimum distance/time it takes to get from one city to another (Dijkstra's Algorithm)
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <sstream>
#include <limits>
#include <stack>
using namespace std;

//Here is the city class, which stores a city's name, type, zone, population, latitude, and longitude.  This class can also handle a vector of cities,
//as well as cooresponding funcitons to access these city values.
class city{
	public:
		string get_name(){
			return this->name;
		}
		string get_type(){
			return this->type;
		}
		unsigned int get_zone(){
			return this->zone;
		}
		unsigned int get_pop(){
			return this->population;
		}
		unsigned int get_csize(){
			return this->cities.size();
		}
		const float get_lat(){
			return this->latitude;
		}
		const float get_long(){
			return this->longitude;
		}
		vector<city>& get_cities(){
			return this->cities;
		}
		city& operator[](int i){
			return this->cities[i];
		}

		friend istream &operator>>(istream &, city &);
		friend ostream &operator<<(ostream &, const city &);

	private:
		string name, type;
		int zone, population;
		float latitude, longitude;
		vector<city> cities;
};

//These 2 functions change a radian value to degrees and vice versa.
inline float degtorad(const float&);
inline float radtodeg(const float&);

inline float degtorad(const float& deg){
    return deg * (M_PI / 180);
}

inline float radtodeg(const float& rad){
    return rad * (180 / M_PI);
}

//Here is the overloaded>> operator to read in city data from an istream.
istream &operator>>(istream &in, city &c){
	in >> c.zone >> c.name >> c.type >> c.latitude >> c.longitude >> c.population;

	//degrees to rad
	c.latitude = degtorad(c.latitude);
	c.longitude = degtorad(c.longitude);

	return in;
}

//Here is the overloaded<< operator to write out city data.
ostream &operator<<(ostream &out, const city &c){
	return(out << setw(17) << left << c.name << " "
			<< setw(11) << left << c.type << " "
			<< setw(1) << left << c.zone << " "
			<< setw(10) << right << c.population << " "
			<< setw(7) << right << fixed << setprecision(2) << radtodeg(c.latitude) << " "
			<< setw(7) << right << fixed << setprecision(2) << radtodeg(c.longitude));
}

//Here is the travelcost class, which holds a table of distances between cities, as well as a table of times between cities.
class travelcost{
	public:
		travelcost(city&);
		float& operator()(int mode, int i, int j);
		unsigned int get_dsize(){
			return this->distance_table.size();
		}
		unsigned int get_tsize(){
			return this->time_table.size();
		}
	private:
		vector<float> distance_table;
		vector<float> time_table;
};

//This function is used to compute distances between locations on earth using coordinates.
inline float haversine(const float &x1, const float &y1, const float &x2, const float &y2, const float &rad){
    //compute deltas
    float delta[2];
    delta[0] = fabsf(y2 - y1);
    delta[1] = fabsf(x2 - x1);

    return 25.0f * round(
            2 * asin(
            sqrt(
            pow(sin(delta[0] / 2.0f), 2.0f) + (
            cos(y1) *
            cos(y2) *
            pow(sin(delta[1] / 2.0f), 2.0f)
            )
            )
            ) * rad / 25.0f);
}

//Here is the constructor for travelcost that creates the distance and time vectors given the city data.
travelcost::travelcost(city &c){
	unsigned int i, j;
	float distance;
	const float rad = 3982.0f;
	
	for(i = 1; i < c.get_csize(); i++){
		distance_table.push_back(0.0f);
		time_table.push_back(0.0f);
		for(j = 0; j < i; j++){
			distance = haversine(c[i].get_long(), c[i].get_lat(), c[j].get_long(), c[j].get_lat(), rad);
			distance = 25.0 * round(distance / 25.0);
			distance_table.push_back(distance);
			if(c[i].get_zone() == c[j].get_zone() && c[i].get_type() == "REGIONAL" && c[j].get_type() == "REGIONAL"){
				time_table.push_back(distance / 65.0);
			}
			else{
				time_table.push_back(distance / 520.0);
			}
		}
	}
}

//This is travelcost's overloaded () operator that returns distance or time values depending on what "mode" and index values are given.
float& travelcost::operator()(int mode, int i, int j){
	int ti, tj;
	if(mode == 0){
		if(i < j) {
			ti = j;
			tj = i;
			return distance_table[((ti * (ti + 1)) / 2) + tj];
		}
		else{
			ti = i;
			tj = j;
			return distance_table[((ti * (ti + 1)) / 2) + tj];
			}
	}
	else{
		if(i < j) {
			ti = j;
			tj = i;
			return time_table[((ti * (ti + 1)) / 2) + tj];
		}
		else{
			ti = i;
			tj = j;
			return time_table[((ti * (ti + 1)) / 2) + tj];
		}
	}
}

void read_cityinfo(vector<city> &);

//Here is the dijkstra_route code that determines the shortest route between two cities.  It first translates the given city names into index values, then processes
//the data to find the best route.  I also embedded the show_route function at the bottom to just go ahead and print out the route this function took.
void dijkstra_route(string &s1, string &s2, travelcost &tc, int &mode, vector<city> &c_vec, vector<vector<int> > &graph){
	int i, j;
	int source, sink;
	for(i = 0; i < (int)c_vec.size(); i++){
		if(c_vec[i].get_name() == s1){
			source = i;
		}
		if(c_vec[i].get_name() == s2){
			sink = i;
		}
	}

	vector<int> vcolor;
	vcolor.assign(c_vec.size(), 0);

	vector<float> vdist;
	vdist.assign(c_vec.size(), numeric_limits<float>::max());
	vdist[source] = 0.0f;

	vector<int> vlink;
	vlink.assign(c_vec.size(), -1);
	vlink[source] = source;

	while(1){
		int next_i = -1;
		float mindist = numeric_limits<float>::max();

		for(i = 0; i < (int)vcolor.size(); i++){
			if(vcolor[i] == 0 && mindist > vdist[i]){
				next_i = i;
				mindist = vdist[i];
			}
		}

		int i = next_i;
		if(i == -1){
			//return;
			break;
		}

		vcolor[i] = 1;

		if(i == sink){
			break;
		}

		for(int k = 0; k < (int)graph[i].size(); k++){
			j = graph[i][k];
			float wij = tc(mode, i, j);
			if(vcolor[j] == 0){
				if(vdist[j] > vdist[i] + wij){
					vdist[j] = vdist[i] + wij;
					vlink[j] = i;
				}
			}
		}
	}

	//Display route
	stack<int> S;

	for(i = sink; i != source; i = vlink[i]){
		S.push(i);
	}
	S.push(source);
	float tm = 0.0f;
	float tt = 0.0f;
	while(!S.empty()){
		i = S.top();
		S.pop();
		tm = tm + tc(0, source, i);
		tt = tt + tc(1, source, i);
	//	source = i;
		//cout << i << " " << c_vec[i].get_name() << " " << vdist[i] << "\n";
		cout << setprecision(1) << fixed << tm << " miles   " << setprecision(1) << fixed << tt << " hours  : " << i << " " << c_vec[i].get_name() << "   " << setprecision(1) << fixed << tc(0, source, i) << " miles   " << setprecision(1) << fixed << tc(1, source, i) << " hours" << "\n";
	source = i;
	}
}

//Here is the code to create the graph of cities, aka the edges between the cities.  It takes in a vector of int vectors, as well as city and travelcost date, and 
//creates the graph based on conditions given in the handout.
void create_citygraph(vector<city> &c_vec, travelcost &tc, vector<vector<int> > &graph){
	size_t g_sz, z_c;
	int k;
	unsigned int i, j;

	g_sz = c_vec.size();

	//get number of zones
	z_c = 0;
	for(i = 0, j = 0; i < g_sz; i++){
		if(c_vec[i].get_zone() != j){
			z_c++;
			j = c_vec[i].get_zone();
		}
	}
	
	//account for 0 not being a valid zone
	z_c++;

	graph.clear();
	graph.resize(g_sz, vector<int>());

	for(i = 0; i < g_sz; i++){
		if(c_vec[i].get_type() == "REGIONAL"){
			//Gateway index
			k = -1;

			//Find closest gateway city in the zone
			for(j = 0; j < g_sz; j++){
				if(c_vec[j].get_type() == "GATEWAY" && c_vec[i].get_zone() == c_vec[j].get_zone() && (k == -1 || tc(0, i, j) < tc(0, i, k))){
					k = j;
				}
			}

			//Add all regional cities, as well as the gateway the moment "k" is hit
			for(j = 0; j < g_sz; j++){
				if(i == j){
					continue;
				}

				//Regional city in the same zone
				if(c_vec[j].get_type() == "REGIONAL" && c_vec[i].get_zone() == c_vec[j].get_zone()){
					graph[i].push_back(j);
					graph[j].push_back(i);
				}

				//Add closest gateway city
				if(j == k){
					graph[i].push_back(j);
					graph[j].push_back(i);
				}
			}
		}
		else{
			//Gateway index
			unsigned int z;
			vector<int> k_vec;

			//Find closest gateway cities in other zones
			for(z = 0; z < z_c; z++){
				k = -1;
				for(j = 0; j < g_sz; j++){
					if(c_vec[j].get_type() == "GATEWAY" && z == c_vec[j].get_zone() && z != c_vec[i].get_zone() && (k == -1 || tc(0, i, j) < tc(0, i, k)) && tc(0, i, j) < 6000.0){
						k = j;
					}
				}
				k_vec.push_back(k);
			}

			//Add all Gateway cities, as well as the gateway the moment "k" is hit
			for(j = 0; j < g_sz; j++){
				if(i == j){
					continue;
				}

				//Gateway city in the same zone
				if(c_vec[j].get_type() == "GATEWAY" && c_vec[i].get_zone() == c_vec[j].get_zone()){
					graph[i].push_back(j);
					graph[j].push_back(i);
				}

				//Add closest gateway cities of other zone
				vector<int>::iterator it;
				for(it = k_vec.begin(); it != k_vec.end(); ++it){
					if(j == *it){
						graph[i].push_back(j);
						graph[j].push_back(i);
					}
				}
			}
		}
	}
	//Sort graph indices and erase duplicate entries.
	for(unsigned int n = 0; n < graph.size(); n++){
		sort(graph[n].begin(), graph[n].end());
		graph[n].erase(unique(graph[n].begin(), graph[n].end()), graph[n].end());
	}
}

//This function reads in the city info using the overloaded >> operator.
void read_cityinfo(vector<city> &cities){
	ifstream fp;
	string s, test;
	city temp;

	fp.open("city_list.txt");
	if(!(fp.is_open())){
		cerr << "Error: File could not be open to read\n";
		return;
	}
	
	while(getline(fp, s)){
		if(s[0] != '#' && !(s.empty())){
			istringstream iss(s);
			iss >> temp;
			cities.push_back(temp);
		}
	}
	fp.close();
}

//This function writes out the city vector to show its information.
void write_cityinfo(vector<city> &c_vec, ostream &out){
	unsigned int i, sz;

	sz = c_vec.size();

	out << "CITY INFO (N=" << sz << "):\n\n";

	for (i = 0; i < sz; i++){
		const city &c_ref = c_vec[i];

		out << setw(3) << i << " " << c_ref << endl;
	}
}

//This is the function that can be used in int main to more easily write the city info.
void write_cityinfo(vector<city> &c_vec){
	ofstream op("city_info.txt");
	write_cityinfo(c_vec, op);
	op.close();
}

//This is the function that writes out the travel distances of each city to a file.
void write_traveldistance(travelcost &tc, vector<city> &c_vec){
	ofstream op("city_distancetable.txt");
	unsigned int i, j, count, sz;
	
	op << "DISTANCE TABLE:" << endl << endl;
	
	count = 1;
	sz = c_vec.size();
	
	//setw(38 - x) << setfill
	for(i = 1; i < sz; i++){
		for(j = 0; j < i; j++){
			unsigned int x = c_vec[i].get_name().size() + 4;
			op << "  " << count << " " << c_vec[i].get_name() << " to " << setfill('.') << setw(38 - x) << left << c_vec[j].get_name() << " " <<  setprecision(1) << fixed << tc(0, i, j) << " miles" << endl;
		}
		op << endl;
		count++;
	}
	op.close();
		
}

//This is the function that writes out the travel times of each city to a file.
void write_traveltime(travelcost &tc, vector<city> &c_vec){
    ofstream op("city_timetable.txt");
    unsigned int i, j, count, sz;

    op << "TIME TABLE:" << endl << endl;

    count = 1;
    sz = c_vec.size();

    for(i = 1; i < sz; i++){
        for(j = 0; j < i; j++){
            unsigned int x = c_vec[i].get_name().size() + 4;
            op << "  " << count << " " << c_vec[i].get_name() << " to " << setfill('.') << setw(38 - x) << left << c_vec[j].get_name() << " " <<  setprecision(1) << fixed << tc(1, i, j) << " hours" << endl;
        }
        op << endl;
        count++;
    }

    op.close();

}

//This is the function to write out the city graph that was made to a file.
void write_citygraph(vector<vector<int> > &graph, travelcost &tc, vector<city> &c_vec){
	ofstream op("city_graph.txt");
	unsigned int i, count, sz;
	vector<int>::iterator it;

	op << "CITY GRAPH:" << endl << endl;

	count = 0;
	sz = c_vec.size();

	for(i = 0; i < sz; i++){
		op << count << " " << c_vec[i].get_name() << endl;
		for(it = graph[i].begin(); it != graph[i].end(); ++it){
			op << "   " << *it << " " << c_vec[*it].get_name() << "    " << setprecision(1) << fixed << tc(0, i, *it) << " miles   " << setprecision(1) << fixed << tc(1, i, *it) << " hours"<< endl;
		}
		count++;
		op << endl;
	}
	op.close();
}

//In int main, we decode the command line option to choose between "-graphinfo", "-distance", or "-time".  If it is "-graphinfo", make the city_info, city_distancetable,
//city_timetable, and city_graph text files using a list of cities and the functions above.  If it is "-distance" or "-time", we use Dijkstra's algorithm to determine the 
//shortest path between two given cities, either based on distance or time.
int main(int argc, char *argv[]){
  if(argc == 1 || argc >=  3){
        cerr << "usage: ./citysim -graphinfo|-distance|-time" << endl;
        return 0;
  }
  else{
      if(argc == 2 && (strcmp(argv[1], "-graphinfo") != 0 && strcmp(argv[1], "-distance") != 0 && strcmp(argv[1], "-time") != 0)){
              cerr << "usage: ./citysim -graphinfo|-distance|-time" << endl;
              return 0;
      }
  }

 // option decoding
	int mode = -1;
	if(strcmp(argv[1], "-distance") == 0){
		mode = 0;
	}
	if(strcmp(argv[1], "-time") == 0){
		mode = 1;
	}
  //object declarations
  city gcity;
  vector<vector<int> > graph;

  //read_cityinfo()
  read_cityinfo(gcity.get_cities());
  write_cityinfo(gcity.get_cities());

  //set up travelcosts
  travelcost tc(gcity);
  write_traveldistance(tc, gcity.get_cities());
  write_traveltime(tc, gcity.get_cities());

  //create_citygraph()
  create_citygraph(gcity.get_cities(), tc, graph);
  write_citygraph(graph, tc, gcity.get_cities());
  
  //Dijkstra_route
  if(mode != -1){
	string n1, n2;
	while(true){
		cin >> n1 >> n2;
		  dijkstra_route(n1, n2, tc, mode, gcity.get_cities(), graph);
	}
  }
}
