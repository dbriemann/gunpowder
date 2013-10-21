#ifndef MCTSENGINE_HPP
#define MCTSENGINE_HPP

#include <vector>
#include <cmath>
#include <unordered_map>
using namespace std;

#include "definitions.hpp"
#include "FastBoard.hpp"
#include "Timer.hpp"

//#define PLAIN_NODE_SIZE 90


//static U64 MCTS_used_nodes = 0ULL;


struct MCTNode {
    U32 wins;
    U32 visits;
    U32 mem_pos;
    U8 move;
    U8 color;
    bool children_expanded;
    MCTNode *parent_node;
    unordered_map<U8, MCTNode *> child_nodes;

    MCTNode(int mem_pos);
//    MCTNode(MCTNode *parent, FastBoard &board, U8 lmove, int mem_pos);
    ~MCTNode();
    MCTNode(const MCTNode &other);

    void update(U8 result);
    MCTNode * selectChildUCT();
    void print(int depth);
};




MCTNode::MCTNode(int mem_pos) {
    this->mem_pos = mem_pos;
    wins = 0;
    visits = 0;
    move = 0;
    color = BLACK; //color that made "lmove"
    parent_node = NULL;
    child_nodes = unordered_map<U8, MCTNode *>();
    for(int m = FIRST_FIELD; m <= LAST_FIELD; m++) {
        //create children for first move node
        child_nodes[m] = NULL;
    }
    children_expanded = false;
//    MCTS_used_nodes++;
}

//MCTNode::MCTNode(MCTNode *parent, FastBoard &board, U8 lmove, int mem_pos) {
//    this->mem_pos = mem_pos;
//    wins = 0;
//    visits = 0;
//    child_nodes = unordered_map<U8, MCTNode *>();
//    move = lmove;
//    parent_node = parent; //NULL if root
//    color = FLIP(board.to_play); //color that made "lmove"

//    //shuffle vector.. so taking and removing a random element is only pop_back stuff in the simulation phase
////    random_shuffle(board.possible_moves.begin(), board.possible_moves.end());

//    //create child entries with NULL pointers
//    for(U8 m : board.possible_moves) {
//        child_nodes[m] = NULL;
//    }
//    children_expanded = false;

////    MCTS_used_nodes++;
//}

MCTNode::MCTNode(const MCTNode &other) {
    mem_pos = other.mem_pos;
    wins = other.wins;
    visits = other.visits;
    move = other.move;
    color = other.color;
    parent_node = other.parent_node;
    child_nodes = other.child_nodes;
    children_expanded = other.children_expanded;
//    MCTS_used_nodes++;
}

MCTNode::~MCTNode() {
    parent_node = NULL;
    for(auto &p : child_nodes) {
        delete p.second;
        p.second = NULL;
    }
//    MCTS_used_nodes--;
}

void MCTNode::print(int depth) {
    for(int i = 0; i < depth; i++) {
        cerr << "   ";
    }
    cerr << (int)move << "| " << (int)color << " |   [" << this << "](" << parent_node << ")" << endl;
    for(auto &p : child_nodes) {
        p.second->print(depth+1);
    }
}

void MCTNode::update(U8 result) {
    visits++;
    wins += result;
}

MCTNode * MCTNode::selectChildUCT() {
    MCTNode *child = child_nodes.begin()->second;

    double ltvis = log(this->visits);
    double val = ((double)child->wins / (double)child->visits) + sqrt(2.0 * ltvis / (double)child->visits);

    for(auto &p : child_nodes) {

        double nval = ((double)p.second->wins / (double)p.second->visits) + sqrt(2.0 * ltvis / (double)p.second->visits);

        if(nval > val) {
            val = nval;
            child = p.second;
        }
    }

    return child;
}



#define TOTAL_NODES 100000

struct NodeManager {
    MCTNode * nodes[TOTAL_NODES];
    bool active_nodes[TOTAL_NODES];
    U32 available_nodes;
    U32 next_node_at;

    NodeManager();

    MCTNode * newNode(MCTNode *parent, FastBoard &board, U8 lmove);
    void pruneUseless(MCTNode *root);
    void invalidateDescending(MCTNode *root);
};

NodeManager::NodeManager() {
    available_nodes = TOTAL_NODES;
    next_node_at = 0;
    for(int i = 0; i < TOTAL_NODES; i++) {
        active_nodes[i] = false;
        nodes[i] = new MCTNode(i);
    }
}

void NodeManager::invalidateDescending(MCTNode *root) {
    active_nodes[root->mem_pos] = false;

    for(auto &p : root->child_nodes) {
        if(p.second != NULL) {
            invalidateDescending(p.second);
        }
    }
}

void NodeManager::pruneUseless(MCTNode *root) {
    if(available_nodes <= 1) {
        U8 worst_next_move = 0;
        double worst_score = 1.0;

        //TODO
        //descent if there are less children than X

        for(auto &next_node : root->child_nodes) {
            double score = (double)next_node.second->wins / (double)next_node.second->visits;
            if(score < worst_score) {
                worst_next_move = next_node.first;
                worst_score = score;
            }
        }

        //now prune the worst branch from the tree
        invalidateDescending(root->child_nodes[worst_next_move]);

        root->child_nodes.erase(worst_next_move);
    }
}

MCTNode * NodeManager::newNode(MCTNode *parent, FastBoard &board, U8 lmove) {
    MCTNode *ret_node = NULL;

    if(available_nodes > 2) {
        //make node active
        active_nodes[next_node_at] = true;

        //the creation of a "new" node is just reassigning an old node to new values
        ret_node = nodes[next_node_at];
        ret_node->parent_node = parent;
        ret_node->wins = 0;
        ret_node->visits = 0;
        ret_node->move = lmove;
        ret_node->color = FLIP(board.to_play);
        ret_node->child_nodes.clear();

        //create all children of this move with NULL pointer
        for(U8 m : board.possible_moves) {
            ret_node->child_nodes[m] = NULL;
        }
        ret_node->children_expanded = false;

        available_nodes--;

        //find next available node
        while(active_nodes[next_node_at]) {
            next_node_at++;
        }
    }

    return ret_node;
}


static NodeManager node_manager;

struct MCTSEngine {
    FastBoard root_state;
    FastBoard sim_state;
    MCTNode *root_node;
    WallTimer wtimer;
    U32 simulations;

    MCTSEngine();
    MCTSEngine(FastBoard &root_state);

    void makePermanentMove(U8 idx);
    U8 runSim(double remaining_time);
    void initRoot(FastBoard &rstate);
    void expand(MCTNode *root, FastBoard &board);
};

MCTSEngine::MCTSEngine() {
    simulations = 0;
    root_state = FastBoard();
    root_node = node_manager.newNode(NULL, root_state, 0);
    node_manager.pruneUseless(root_node);
    expand(root_node, root_state);
}

MCTSEngine::MCTSEngine(FastBoard &game_state) {
    simulations = 0;
    this->root_state = game_state;
    root_node = node_manager.newNode(NULL, root_state, 0);
    node_manager.pruneUseless(root_node);
    expand(root_node, root_state);
}


void MCTSEngine::expand(MCTNode *root, FastBoard &board) {
    FastBoard cpy;

    for(auto &entry : root->child_nodes) {
        cpy = board;
        cpy.makeMove(entry.first);

        root->child_nodes[entry.first] = node_manager.newNode(root, cpy, entry.first);
        node_manager.pruneUseless(root_node);
    }

    root->children_expanded = true;
}

void MCTSEngine::makePermanentMove(U8 idx) {
    cerr << "*** Nodes before pruning: " << TOTAL_NODES - node_manager.available_nodes << endl;
    root_state.makeMove(idx);

    //prune leftover part of tree
    MCTNode *old_root = root_node;
    root_node = NULL;

    //remove new root from old roots children
    //and set it to new root
    auto iter = old_root->child_nodes.find(idx);
    if(iter != old_root->child_nodes.end()) {
//        cerr << "*** OLD NODE AS ROOT" << endl;
        root_node = iter->second;
        root_node->parent_node = NULL;
        old_root->child_nodes.erase(idx);
    }

    node_manager.invalidateDescending(old_root);

    old_root = NULL;
    cerr << "*** New root: " << root_node << " move: " << (int)root_node->move << endl;
    cerr << "*** Nodes after pruning: " << TOTAL_NODES - node_manager.available_nodes << endl;
}



U8 MCTSEngine::runSim(double remaining_time) {
    wtimer.start();
    simulations = 0ULL;
    MCTNode *node = NULL;

    double turn_time = remaining_time / 10.0;

    cerr << "   --- Run simulation for move no. " << (int)root_state.next_move << " ---" << endl;
    cerr << "   --- Turn time: " << turn_time << " sec." << endl;

    //for now never stop searching
    while(!wtimer.out_of_time(turn_time)) {
        simulations++;
        node = root_node;
        sim_state = root_state;

        //selection
        while(node->children_expanded) {
            //cur_node is fully expanded and non-terminal
            node = node->selectChildUCT();
            sim_state.makeMove(node->move);
        }

        //expansion
        expand(node, sim_state);
        //random next move
        U32 r = fastrand() % node->child_nodes.size();
        auto iter = node->child_nodes.begin();
        advance(iter, r);
        node = iter->second;

//        if(!node->untried_moves.empty()) {
//            //there are still child nodes to expand
//            node = node->addRandomChild(sim_state);
//        }

        //simulation
        U8 win_color = sim_state.randomFill();

        //backpropagation
        while(node != NULL) {

            if(win_color == node->color) { //node is win
                node->update(1);
            } else { //node is loss
                node->update(0);
            }

            if(node != root_node) {
                //update siblings with amaf scores
                for(auto sib_iter = node->parent_node->child_nodes.begin(); sib_iter != node->parent_node->child_nodes.end(); sib_iter++) {
                    if(win_color == sim_state.stones[sib_iter->second->move]) {
                        sib_iter->second->update(1);
                    } else {
                        sib_iter->second->update(0);
                    }

                }
            }
            node = node->parent_node;
        }

    }


    U32 most_visits = 0;
    U8 best_move = root_state.possible_moves[0];
    U32 wins = 0;

    cerr << "   --- " << simulations << " simulations run." << endl;
    double score;

    for(auto &p : root_node->child_nodes) {
        if(p.second->visits > most_visits) {
            most_visits = p.second->visits;
            best_move = p.second->move;
            wins = p.second->wins;
        }
        score = (double) p.second->wins / (double) p.second->visits * 100;
        cerr << "   # Move: " << (int) p.second->move << " Score: " << score << "%" << endl;
    }

    cerr << "---> # Best Move: " << (int) best_move << "   visits: " << most_visits << "   wins: " << wins << " == " << (double)wins/(double)most_visits*100.0 << " %" << endl;

    return best_move;
}



#endif // MCTSENGINE_HPP
