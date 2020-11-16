#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <limits.h>//do int_max
#include <cstddef>//do null
#include <unordered_map>
#include <cassert>
#include <stack>

using namespace std;




ostream out(nullptr);
struct Node{
    int magic; // żeby sprawdzić czy nody są poprawne .Numer do debugera
    string name;
    int distance_from_begining;
    Node* poprzednik;


    Node(){
        distance_from_begining=INT_MAX; //int max uznajemy jako "nieskoñczonosc"
        poprzednik=NULL;
        magic=0x44332211;
    }
};

struct Node_comparator {
    int ile_razy;
    bool operator() (const Node* a, const Node* b){
        if(a->distance_from_begining<b->distance_from_begining){
                return true;
        } else if(a->distance_from_begining==b->distance_from_begining){
            if(a->name<b->name){
                return true;
            }
        }


        return false;
    }
};
struct Route{
    Node* start;
    Node* target;
    int distance;

};


static unordered_map<string,Node*> node_factory_map;
//node factory
/*!
    Producing nodes

    Function is checking if Node with the name already exists and if it doesn't, it is added to the node factory map.
    Guarantees that for each city, there is only one object

    @param name name of the city
    @return Node object added to node_factory_map
*/
static Node* produceNode(string name){
     if(name.length()>0){
         if (node_factory_map.find(name)!= node_factory_map.end()) {
            return (*node_factory_map.find(name)).second;
         }
         else{
             Node* tmp= new Node;
             tmp->name=name;
             node_factory_map[name]=tmp;
             return tmp;
         }
     } else{
        return NULL;
     }
}
/*!
    Deleting nodes

    Function is deleting nodes at the end of the program, to avoid memory leak.

    @return void

*/
static void clear_all_nodes(){
    for(unordered_map<string,Node*>::iterator it=node_factory_map.begin();it!=node_factory_map.end();it++){
        delete((*it).second);
    }
    node_factory_map.clear();
}


static set <Node*,Node_comparator> nodes;
static vector<Route> routes;

/*!
    Reading source file

    Function is reading the input file and importing city names and distances. It also checks if nodes have content and adding nodes to the "nodes" set.
    And routes to the "routes" vector
    @param file input file
    @return void

*/
static void read_line(ifstream &file, string start_point){
    do{

        string line;
        Node *n1=NULL;
        Node *n2=NULL;
        Route r1;
        string name_tmp;
        file>>name_tmp;
        n1=produceNode(name_tmp);
        if(n1==NULL){
            break;
        }
        if(file.eof()) break;
        file>>name_tmp;
        n2=produceNode(name_tmp);
        if(n2==NULL){
            break;
        }
        if(n1->name==start_point) n1->distance_from_begining=0; //zamiast (*)- analityczna metoda, użycie podtsawowoe
        if((*n2).name==start_point) n2->distance_from_begining=0; // -> syntax sugar
        r1.distance=-1;
        file>>r1.distance;
        r1.start=n1;
        r1.target=n2;
        nodes.insert(n1);
        nodes.insert(n2);
        routes.push_back(r1);

    }while(false);// po to żeby można bylo zrobic break

}

/*!
    Djikstra's algorithm

    Function is implementing Djikstra's algorithm to find the shortest distance from the starting point to each of the cities.
    The distance is then saved in distance_from_beginning variable.
    It also adds previous node to the Poprzednik variable, which will be used in final_print function


    @return void

*/

static void czyn(){

    while(!nodes.empty()){
        Node* tmp;
        tmp=*nodes.begin();
        nodes.erase(nodes.begin());

        for(vector<Route>::iterator it=routes.begin();it!=routes.end();it++){
            Route &itr=*it;

            if(itr.start==tmp){
                    int alt=itr.start->distance_from_begining+itr.distance;

                if(itr.target->distance_from_begining>alt){
                    nodes.erase(itr.target);
                    itr.target->distance_from_begining=alt;
                    itr.target->poprzednik=itr.start;
                    nodes.insert(itr.target);
                }
            }
            if(itr.target==tmp){ //w druga strone trasy
                    int alt=itr.target->distance_from_begining+itr.distance;

                if(itr.start->distance_from_begining>alt){
                    nodes.erase(itr.start);
                    itr.start->distance_from_begining=alt;
                    itr.start->poprzednik=itr.target;
                    nodes.insert(itr.start);
                }
            }
        }

    }
}
/*!
    Writing to file

    Function writes routes to the output file in the correct way specified in the task description

    @return void

*/
static void final_print(){

string output_file_name;
    for(auto it=node_factory_map.begin();it!=node_factory_map.end();it++){
        Node* tmp=(*it).second;
       int final_distance;
       stack<Node*> s;
       if(tmp!= NULL){
            final_distance=tmp->distance_from_begining;
            while(tmp->distance_from_begining!=0){
                s.push(tmp);
                tmp=tmp->poprzednik;
            }
            s.push(tmp);
        }
        if(final_distance>0){
                bool first=true;
            while (!s.empty()){
                if(first==false){
                    out <<" -> ";
                }
                 out << s.top()->name;
                 s.pop();
                 first=false;
              }
            out <<": "<<final_distance<<endl;
        }
    }

}




int main(int argc, char *argv[])
{
    string output_file_name;
    string input_file_name;
    string start_point;
    out.rdbuf(cout.rdbuf());

    if(argc<6){
            cerr<<"Za mala ilosc argumentow!"<<endl;
        if(argc==1){
            cerr<<"Aby uruchomic program, nalezy podac argumenty: "<<endl;
            cerr<<"-i plik wejsciowy z drogami"<<endl;
            cerr<<"-o plik wyjsciowy z trasami spedycyjnymi"<<endl;
            cerr<<"-s nazwa miasta startowego, gdzie znajduje sie centrala"<<endl;
        }
        return 0;
    }
    for(int i=0;argv[i]!=NULL;i++){
        if(string(argv[i])=="-i"){
            input_file_name=argv[i+1];
            i++;
        }
        else if(string(argv[i])=="-o"){
            output_file_name=argv[i+1];
            i++;
        }else if(string(argv[i])=="-s"){
            start_point=argv[i+1];
            i++;
        }
    }
    ifstream file; //ifstream- input file stream
    ofstream ofile;
    ofile.open(output_file_name.c_str());
    out.rdbuf(ofile.rdbuf());

    file.open(input_file_name.c_str()); //konwertuje file-path na chary
    if(file.fail()){
        cerr<<"nieudane otwarcie pliku: "<<input_file_name<<endl;
        return 1000;
    }
    while(!file.eof()){
      read_line(file,start_point);
    }

    czyn();
    final_print();

    clear_all_nodes();
    routes.clear();
    assert(nodes.size()==0);
    file.close();
    ofile.close();
    cerr<<"plik z trasami zostal utworzony"<<endl;
    return 0;
}
