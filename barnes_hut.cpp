#include <iostream>
#include <fstream>
#include <iomanip>
#include <tbb/tbb.h>
#include <chrono>
#define THREADS_N 1

const double G = 6.67e-11;
const double d_t = 1;
int bodies_n;
double space_dim;
class BHNode;

using namespace std;

typedef struct point {
   double x,y;  
} Point;


typedef struct quadrant {
    Point min;
    Point max;
    bool contains(BHNode * n);
    double length() {
        return abs(this->max.x - this->min.x);
    }

} Quadrant;

class BHNode {
    public:
        Point position;
        Point velocity;
        Point acceleration;
        double mass;
        string name;
        
        int quad_index;
        Quadrant quad;
        bool is_body;
        vector<BHNode *> children;

        double x_sum; //for center of mass calculation
        double y_sum;



        BHNode(): is_body(false), name(""), children(4, nullptr), position{0, 0}, mass(0), x_sum(0), y_sum(0) {}

        BHNode(Quadrant quad): is_body(false), quad(quad), name(""), children(4, nullptr), position{0, 0}, mass(0), x_sum(0), y_sum(0) {}

        BHNode(double x, double y, double v_x, double v_y, double mass, std::string b_name, bool is_body): 
        position{x, y}, velocity{v_x, v_y}, mass(mass), is_body(is_body), name(std::move(b_name)), acceleration{0, 0} {}

        BHNode(double x, double y, double v_x, double v_y, double mass, std::string b_name, Quadrant q, bool is_body): 
        position{x, y}, velocity{v_x, v_y}, mass(mass), is_body(is_body), name(std::move(b_name)), quad(q), acceleration{0, 0} {}

        int getQuadrantIndex(Quadrant q) {

            Point middle = {(q.min.x + q.max.x)/2, (q.min.y + q.max.y)/2};
            if (position.x > middle.x && position.y > middle.y) return 0; 
            if (position.x <= middle.x && position.y > middle.y) return 1;
            if (position.x <= middle.x && position.y <= middle.y) return 2;
            if (position.x > middle.x && position.y <= middle.y) return 3;
            
            return -1;
        }

        void updateQuadrant(Quadrant q, int index) {
            Point middle = {(q.min.x + q.max.x)/2, (q.min.y + q.max.y)/2};
            if (index==0) {
                this->quad.max.x = q.max.x;
                this->quad.max.y = q.max.y;
                this->quad.min.x = middle.x;
                this->quad.min.y = middle.y;
            }
            if (index==1) {
                this->quad.max.x = middle.x;
                this->quad.max.y = q.max.y;
                this->quad.min.x = q.min.x;
                this->quad.min.y = middle.y;
            }
            if (index==2) {
                this->quad.max.x = middle.x;
                this->quad.max.y = middle.y;
                this->quad.min.x = q.min.x;
                this->quad.min.y = q.min.y;
            }
            if (index==3)  {
                this->quad.max.x = q.max.x;
                this->quad.max.y = middle.y;
                this->quad.min.x = middle.x;
                this->quad.min.y = q.min.y;
            } 

            this->quad_index=index;
        }

        void update_position() {
            this->velocity.x += d_t * this->acceleration.x;
            this->velocity.y += d_t * this->acceleration.y;
            this->position.x += d_t * this->velocity.x;
            this->position.y += d_t * this->velocity.y;
        }

        void print_node(int depth) {
            for (int i = 0; i < depth; i++) cout << "  ";  
            if (is_body) cout << "name: " <<name << " quadrant: x_max "<< this->quad.max.x <<" y_max "<<this->quad.max.y << " x_min " <<this->quad.min.x << " y_min " <<this->quad.min.y<< " quad_index : " << this->quad_index<<endl ;
            else cout << "Internal Node , quadrant: x_max "<< this->quad.max.x <<" y_max "<<this->quad.max.y << " x_min " <<this->quad.min.x << " y_min " <<this->quad.min.y<< " center of mass x " << this->position.x << " y " << this->position.y<< " m " << this->mass<<endl;
        }
};

bool quadrant::contains(BHNode * n) {
    return (n->position.x <= this->max.x && n->position.x >= this->min.x && n->position.y >= this->min.y && n->position.y <= this->max.y);
}


void delete_nodes(BHNode * node) {

    if (node == nullptr) return;

    for (auto& child : node->children) delete_nodes(child);
    
    if (node->is_body==false) delete node;
}

class BHTree {
    public:
        BHNode * root;
        vector<BHNode *> bodies;

        BHTree() {
            root = new BHNode(Quadrant{Point{-space_dim, -space_dim},Point{space_dim, space_dim}});
        }

        ~BHTree() {
            delete_nodes(this->root);
            for (BHNode * b : this->bodies) delete b;
        }

        void print_tree(BHNode* node, int depth = 0) {
            if (node == nullptr) return;
            node->print_node(depth);
            for (auto child : node->children) {
                if (child != nullptr) {
                    print_tree(child, depth + 1);
                } else {
                    for (int i = 0; i < depth+1; i++) cout << "  ";  
                    cout<<"empty child\n";

                }
            }
        }

        void clear_tree() {
            delete_nodes(this->root);
            this->root = new BHNode(Quadrant{Point{-space_dim, -space_dim}, Point{space_dim, space_dim}});
        }
}; 


BHTree * read_input_file(string file_name) {
    ifstream input_file(file_name);

    if (!input_file.is_open()) {
        cerr << "Error opening input file"<<endl;
        exit(EXIT_FAILURE);
    }

    input_file >> bodies_n;
    input_file >> space_dim;

    BHTree * ret = new BHTree();
    double x,y, vx,vy,m;
    string name;

    for (int i=0; i<bodies_n; i++) {
        input_file>>x>>y>>vx>>vy>>m>>name;
        if (abs(x)>space_dim || abs(y)>space_dim) {
            cerr << "Coordinates ("<<x<<", "<<y <<") out of range. Body ignored."<<endl;
        }
        ret->bodies.push_back(new BHNode(x,y,vx,vy,m, name, true));
    }

    return ret;
}

void print_results(string file_name, BHTree * bhtree) {
    ofstream output_file(file_name);

    if (!output_file.is_open()) {
        cerr << "Error opening input file"<<endl;
    }

    output_file <<scientific << setprecision(2) << noshowpos<<bodies_n << endl << space_dim << endl;
    for (const auto& b : bhtree->bodies) {
        output_file << scientific << setprecision(3) << noshowpos << b->position.x <<" "<< b->position.y << " " << b->velocity.x << " " << b->velocity.y << " " << b->mass << " " << b->name<<endl;
    }

    return;
}

void insert(BHNode * root , BHNode * node) {

   int quad_index = node->getQuadrantIndex(root->quad);

   if (quad_index == -1) {
        exit(EXIT_FAILURE);
   }
   
   BHNode * child = root->children.at(quad_index);
   
   if (child == nullptr) { //no body in this quadrant -> place new node there
        node->updateQuadrant(root->quad, quad_index); 
        root->children.at(quad_index) = node;
    } else if (child->is_body == false) { // internal ndoe -> move down and call insert again with that node as root
        insert(child, node); 
    } else { //there is a body in my spot, move it down
        BHNode * new_internal = new BHNode(child->quad); //create new internal node
        root->children.at(quad_index) = new_internal;
        insert(new_internal, child); //insert previous body and new body as children of internal node
        insert(new_internal, node);
    }

    //update center of mass in all nodes passed
    
    root->x_sum += (node->position.x)*(node->mass);    
    root->y_sum += (node->position.y)*(node->mass);
    root->mass += node->mass;
    root->position.x = root->x_sum / root->mass;
    root->position.y = root->y_sum / root->mass;

    return;
}


void construct_tree(BHTree * bhtree) { //constructs tree from bhtree->bodies vector
    for (auto& b : bhtree->bodies) {
        insert(bhtree->root , b);
    }
}


void calculate_force(BHNode * b, BHNode * n, Point * force) {
    if (n->is_body==true) {
        if (n==b) return; //ignore b 
        //force calculation between bodies b and n
        double r = sqrt(pow(n->position.x - b->position.x, 2) + pow(n->position.y - b->position.y, 2));
        double f = (G * (n->mass * b->mass)) / (pow(r,2));
        force->x += (f * (n->position.x - b->position.x))/r;
        force->y += (f*(n->position.y - b->position.y))/r;
    } else {
        double r = sqrt(pow(n->position.x - b->position.x, 2) + pow(n->position.y - b->position.y, 2));
        if (n->quad.contains(b)==false && r>b->quad.length()) { //if n does not contain b and their distance is large enough
            //force calculation between b and n's center of mass
            double f = (G * (n->mass * b->mass)) / (pow(r,2));  
            force->x += (f * (n->position.x  - b->position.x))/r;
            force->y += (f*(n->position.y - b->position.y))/r;
        } else {
            for (auto& child : n->children) { //else move down to n's children recursively
                if (child==nullptr) continue;
                calculate_force(b, child, force);
            }
        }
    }
}




void update_positions(BHTree * bhtree) {

    //calculate forces and acceleration for all bodies
    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, bhtree->bodies.size()), 
        [&bhtree](const tbb::blocked_range<size_t>& range) {
            for (size_t i = range.begin(); i != range.end(); i++) {
                auto& b = bhtree->bodies[i];
                Point * force = new Point{0, 0};
                calculate_force(b, bhtree->root, force);
                b->acceleration.x = force->x / b->mass;
                b->acceleration.y = force->y / b->mass;
                force->x = 0;
                force->y = 0;
                delete force; 
            }
        }
    );

    //update position of all bodies
    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, bhtree->bodies.size()), 
        [&bhtree](const tbb::blocked_range<size_t>& range) {
            for (size_t i = range.begin(); i != range.end(); i++) {
                auto& b = bhtree->bodies[i];
                b->update_position(); 
            }
        }
    );

    //delete and reconstruct bhtree
    bhtree->clear_tree();
    construct_tree(bhtree);
}



int main(int argc, char * argv[]) {

    if (argc!=4) {
        cerr<<"Not enough arguments: Usage: %s, <input_file>, <number_of_iterations>, <output_file>\n", argv[0];
        return 1;
    }
    BHTree * bhtree;

    bhtree = read_input_file(string(argv[1]));
    
    auto start = std::chrono::high_resolution_clock::now();

    construct_tree(bhtree);

    tbb::global_control control(tbb::global_control::max_allowed_parallelism, THREADS_N);

    for (int i=0; i<atoi(argv[2]); i++) {
        update_positions(bhtree);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    print_results(string(argv[3]), bhtree);
    cout<<"Execution time: "<<duration.count()<<" ms"<<endl;

    //bhtree->print_tree(bhtree->root); //debug
    
    return 0;
}


