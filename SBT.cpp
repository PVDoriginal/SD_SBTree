#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <queue>
#include <set>
using namespace std;

ifstream fin("SBT.in");

struct SBTree{
    int root;
    int _rank;
    vector<shared_ptr<SBTree>> children;

    SBTree(int r, int rnk, vector<shared_ptr<SBTree>> c){
        root = r;
        _rank = rnk;
        children = c;
    }

    void Print(){
        cout << "root: " << root << "\n";
        cout << "children: ";
        if(children.size() != 0) for(auto x : children) cout << x->root << ", ";
        else cout << "none";
        cout << "\n";
        for(auto x : children) x->Print();
    }
};
struct CompareRanks{
    bool operator()(const shared_ptr<SBTree> &t1, const shared_ptr<SBTree> &t2){
        return t1->_rank < t2->_rank;
    }
};

struct Heap{
    multiset<shared_ptr<SBTree>, CompareRanks> trees;
    shared_ptr<SBTree> MaxTree; // pointer to the tree containing the Max value

    int GetMax(){
        if(MaxTree == nullptr) return INT_MIN;
        return MaxTree->root;
    }

    void RecalcMax(){
        MaxTree = nullptr;
        for(auto tree : trees)
            if(MaxTree == nullptr || tree->root > MaxTree->root)
                MaxTree = tree;
    }


    void RemoveMax(){
        if(MaxTree == nullptr) return;
        for(auto it = trees.begin(); it != trees.end(); it++)
            if((*it)->root == MaxTree->root && (*it)->_rank == MaxTree->_rank){
                trees.erase(it);
                break;
            }

        shared_ptr<SBTree> tree = MaxTree;

        for(auto child : tree->children)
            if(child->_rank != 0)
                Merge(child);

        for(auto child : tree->children)
            if(child->_rank == 0)
                Insert(child->root);

    }

    void Insert(int val){
        shared_ptr<SBTree> tree(new SBTree(val, 0, vector<shared_ptr<SBTree>>()));
        if(MaxTree == nullptr || tree->root > MaxTree->root) MaxTree = tree;

        if(trees.size() >= 2){
            auto h1 = trees.begin();
            auto h2 = next(trees.begin());

            if((*h1)->_rank == (*h2)->_rank){
                shared_ptr<SBTree> newTree = SkewLink(tree, *h1, *h2);
                trees.erase(h1);
                trees.erase(h2);
                trees.insert(newTree);
                if(MaxTree == *h1 || MaxTree == *h2 || MaxTree == tree) MaxTree = newTree;
                return;
            }
        }
        trees.insert(tree);
    }

    void RemoveDuplicateRanks(){
        if(trees.size() <= 1) return;

        queue<set<shared_ptr<SBTree>>::iterator> toRemove;
        queue<shared_ptr<SBTree>> toAdd;

        bool ok = false;
        while(!ok){
            ok = true;

            for(auto x = next(trees.begin()); x != trees.end(); ++x){
                if((*x)->_rank == (*prev(x))->_rank){
                    shared_ptr<SBTree> tree = SimpleLink(*x, *prev(x));
                    toRemove.push(x);
                    toRemove.push(prev(x));
                    toAdd.push(tree);
                    ok = false;
                }
            }

            for(; !toRemove.empty(); toRemove.pop())
                trees.erase(toRemove.front());
            for(; !toAdd.empty(); toAdd.pop())
                trees.insert(toAdd.front());
        }
    }

    void Merge(Heap &heap){
        RemoveDuplicateRanks();
        heap.RemoveDuplicateRanks();

        trees.insert(heap.trees.begin(), heap.trees.end());
        RemoveDuplicateRanks();
        RecalcMax();
    }

    void Merge(shared_ptr<SBTree> tree){
        Heap heap;
        heap.trees.insert(tree);
        Merge(heap);
    }


    //                                          rank r                rank r
    shared_ptr<SBTree> SimpleLink(shared_ptr<SBTree> tree1, shared_ptr<SBTree> tree2){
        if(tree1->root < tree2->root) tree1.swap(tree2);

        vector<shared_ptr<SBTree>> new_children;
        new_children.push_back(tree2);
        for(auto child : tree1->children)
            new_children.push_back(child);

        return shared_ptr<SBTree>(new SBTree(tree1->root, tree1->_rank+1, new_children));
    }
    //                                        rank 0            rank r                      rank r
    shared_ptr<SBTree> SkewLink(shared_ptr<SBTree> tree0, shared_ptr<SBTree> tree1, shared_ptr<SBTree> tree2){

        if(tree0->root >= tree1->root && tree0->root >= tree2->root){
            vector<shared_ptr<SBTree>> newChildren;
            newChildren.push_back(tree1);
            newChildren.push_back(tree2);
            return shared_ptr<SBTree>(new SBTree(tree0->root, tree1->_rank+1, newChildren));
        }

        if(tree1->root >= tree2->root){
            vector<shared_ptr<SBTree>> newChildren;
            newChildren.push_back(tree0);
            newChildren.push_back(tree2);
            for(auto child : tree1->children) newChildren.push_back(child);
            return shared_ptr<SBTree>(new SBTree(tree1->root, tree1->_rank+1, newChildren));
        }

        vector<shared_ptr<SBTree>> newChildren;
        newChildren.push_back(tree0);
        newChildren.push_back(tree1);
        for(auto child : tree2->children) newChildren.push_back(child);
        return shared_ptr<SBTree>(new SBTree(tree2->root, tree1->_rank+1, newChildren));
    }

    void Print(){
        cout << trees.size() << " trees\n\n";
        for(auto tree : trees){
            cout << "1 Tree of rank " << tree->_rank << ":\n";
            tree->Print();
            cout << "\n";
        }
    }
};

int main(){

    int nr_heaps, nr_queries;
    vector<Heap> heaps;

    fin >> nr_heaps;
    for(int i = 0; i < nr_heaps; i++)
        heaps.push_back(Heap());

    fin >> nr_queries;
    for(int i = 0, type; i < nr_queries; i++){
        fin >> type;

        if(type == 1){
            int heap, val;
            fin >> heap >> val;
            heaps[heap-1].Insert(val);
        }
        else if(type == 2){
            int heap;
            fin >> heap;
            cout << heaps[heap-1].GetMax() << "\n";
            heaps[heap-1].RemoveMax();
        }
        else if(type == 3){
            int heap1, heap2;
            fin >> heap1 >> heap2;
            heaps[heap1-1].Merge(heaps[heap2-1]);
        }
    }
    //heaps[0].Print();
}
