#ifndef MCTS_HPP
#define MCTS_HPP

#include <vector>
#include <cmath>
#include <unordered_map>
using namespace std;

#include "definitions.hpp"
#include "Board.hpp"
#include "Timer.hpp"

static U64 MCTS_used_nodes = 0ULL;

struct MCTNode {
    U32 wins;
    U32 visits;
    U8 move;
    U8 color;
    vector<U8> untried_moves;
    MCTNode *parent_node;
//    vector<MCTNode *> child_nodes;
    unordered_map<U8, MCTNode *> child_nodes;

    MCTNode();
    MCTNode(MCTNode *parent, Board &board, U8 lmove);
    ~MCTNode();
    //TODO COPY CONSTRUCTOR

    void update(U8 result);
    MCTNode * addRandomChild(Board &board);
    MCTNode * selectChildUCT();
    void print(int depth);
};

//TODO --- AMAF
//create all root children at the beginning
//TODO --- SELECT
//TODO shuffle vector and get next instead of rand()

MCTNode::MCTNode() {
    wins = 0;
    visits = 0;
    move = 0;
    color = WHITE;
    untried_moves = vector<U8>();
    parent_node = NULL;
    child_nodes = unordered_map<U8, MCTNode *>();
    MCTS_used_nodes++;
}

MCTNode::MCTNode(MCTNode *parent, Board &board, U8 lmove) {
    wins = 0;
    visits = 0;
    move = lmove;
    color = FLIP(board.to_play); //color that made "lmove"
    untried_moves = board.possible_moves;
    //shuffle vector.. so taking and removing a random element is only pop_back stuff
    random_shuffle(untried_moves.begin(), untried_moves.end());
    parent_node = parent; //NULL if root
    child_nodes = unordered_map<U8, MCTNode *>();
    MCTS_used_nodes++;
}

void MCTNode::print(int depth) {
    for(int i = 0; i < depth; i++) {
        cerr << "   ";
    }
    cerr << (int)move << endl;
    for(auto &p : child_nodes) {
        p.second->print(depth+1);
    }
}

MCTNode::~MCTNode() {
    parent_node = NULL;
    for(auto &p : child_nodes) {
        delete p.second;
        p.second = NULL;
    }
    MCTS_used_nodes--;
}

void MCTNode::update(U8 result) {
    visits++;
    wins += result;
}

MCTNode * MCTNode::addRandomChild(Board &board) {
    U8 lmove = untried_moves.back();
    untried_moves.pop_back();
    board.makeMove(lmove);

    child_nodes[lmove] = new MCTNode(this, board, lmove);
    return child_nodes[lmove];
}

MCTNode * MCTNode::selectChildUCT() {
    MCTNode *child = child_nodes.begin()->second;

    double ltvis = log(this->visits);
    double val = (double)child->wins / (double)child->visits + sqrt(2.0 * ltvis / (double)child->visits);

    for(auto &p : child_nodes) {

        double nval = (double)p.second->wins / (double)p.second->visits + sqrt(2.0 * ltvis / (double)p.second->visits);

        if(nval > val) {
            val = nval;
            child = p.second;
        }
    }
    return child;
}



struct MCTSEngine {
    Board root_state;
    Board sim_state;
    MCTNode *root_node;
    WallTimer wtimer;
    U32 simulations;

    MCTSEngine();
    MCTSEngine(Board &root_state);

    void makePermanentMove(U8 idx);
    U8 runSim(double remaining_time);
//    U8 getRandomUntried();
    void updateAMAF(U8 idx);
    void initRoot();
//    void colorFlip();
};

MCTSEngine::MCTSEngine() {
    simulations = 0;
    root_state = Board();
    root_node = new MCTNode(NULL, root_state, 0);
}

MCTSEngine::MCTSEngine(Board &game_state) {
    simulations = 0;
    this->root_state = game_state;
    root_node = new MCTNode(NULL, root_state, 0);
}


void MCTSEngine::initRoot() {

}

void MCTSEngine::updateAMAF(U8 idx) {

}

//U8 MCTSEngine::getRandomUntried() {
//    int moves = sim_state.possible_moves.size();
//    int rand_idx = rand() % moves;
//    return sim_state.possible_moves[rand_idx];
//}

void MCTSEngine::makePermanentMove(U8 idx) {
    cerr << "Nodes before pruning: " << MCTS_used_nodes << endl;
    root_state.makeMove(idx);

    //prune leftover part of tree
    MCTNode *old_root = root_node;
    root_node = NULL;

    //remove new root from old roots children
    //and set it to new root
    auto iter = old_root->child_nodes.find(idx);
    if(iter != old_root->child_nodes.end()) {
        cerr << "*** OLD" << endl;
        root_node = iter->second;
        root_node->parent_node = NULL;
        old_root->child_nodes.erase(idx);
    }

    //node was not expanded
    if(root_node == NULL) {
        cerr << "*** NEW" << endl;
        root_node = new MCTNode(NULL, root_state, idx);
    }

    //clean up memory
    delete old_root;
    old_root = NULL;
    cerr << "New root: " << root_node << " move: " << (int)root_node->move << endl;
    cerr << "Nodes after pruning: " << MCTS_used_nodes << endl;
//    root_node->print(0);
}



U8 MCTSEngine::runSim(double remaining_time) {
//    MCTNode root_node(NULL, root, 0);
    wtimer.start();
    simulations = 0ULL;
    MCTNode *node = NULL;

    //for now never stop searching
    while(!wtimer.out_of_time(remaining_time / 10.0)) {
        simulations++;
        node = root_node;
        sim_state = root_state;

        //prior knowledge?!

        //selection
        while(node->untried_moves.empty() && !node->child_nodes.empty()) {
            //cur_node is fully expanded and non-terminal
            node = node->selectChildUCT();
            sim_state.makeMove(node->move);            
        }

        //expansion
        if(!node->untried_moves.empty()) {
            //there are still child nodes to expand
//            U8 nextm = getRandomUntried();
//            sim_state.makeMove(nextm);
            node = node->addRandomChild(sim_state);
        }

        //simulation
        I8 win = NONE;
        while(!sim_state.possible_moves.empty() && win == NONE) {
            win = sim_state.makeMove(sim_state.getRandomMove());
        }

        //backpropagation
        while(node != NULL) {
            if(win == WHITE_WIN) {
                if(node->color == WHITE) {
                    node->update(1);
                } else {
                    node->update(0);
                }
            } else { //win black
                if(node->color == BLACK) {
                    node->update(1);
                } else {
                    node->update(0);
                }
            }

            node = node->parent_node;
        }

    }


    U32 most_visits = 0;
    U8 best_move = root_state.possible_moves[0];
    U32 wins = 0;

    for(auto &p : root_node->child_nodes) {
        if(p.second->visits > most_visits) {
            most_visits = p.second->visits;
            best_move = p.second->move;
            wins = p.second->wins;
        }
        cerr << "Node: " << (int)p.second->move << " visits: " << p.second->visits << " wins: " << p.second->wins << " == " << (double)p.second->wins/(double)p.second->visits*100.0 << " %" << endl;
    }

    cerr << "### Total Simulations: " << simulations << endl;
    cerr << "---> Best Move: " << (int) best_move << "   visits: " << most_visits << "   wins: " << wins << " == " << (double)wins/(double)most_visits*100.0 << " %" << endl;

    return best_move;
}


#endif // MCTS_HPP
