#ifndef MCTS_HPP
#define MCTS_HPP

#include <vector>
#include <cmath>
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
    vector<MCTNode *> child_nodes;

    MCTNode();
    MCTNode(MCTNode *parent, Board &board, U8 lmove);
    ~MCTNode();
    //TODO COPY CONSTRUCTOR

    void update(U8 result);
    MCTNode * addChild(U8 lmove, Board &board);
    MCTNode * selectChildUCT();
    void print(int depth);
};

//TODO --- AMAF
//create all root children at the beginning
//TODO --- SELECT

MCTNode::MCTNode() {
    wins = 0;
    visits = 0;
    move = 0;
    color = WHITE;
    untried_moves = vector<U8>();
    parent_node = NULL;
    child_nodes = vector<MCTNode *>();
    MCTS_used_nodes++;
}

MCTNode::MCTNode(MCTNode *parent, Board &board, U8 lmove) {
    wins = 0;
    visits = 0;
    move = lmove;
    color = FLIP(board.to_play); //color that made "lmove"
    untried_moves = board.possible_moves;
    parent_node = parent; //NULL if root
    child_nodes = vector<MCTNode *>();
    MCTS_used_nodes++;
}

void MCTNode::print(int depth) {
    for(int i = 0; i < depth; i++) {
        cerr << "   ";
    }
    cerr << (int)move << endl;
    for(MCTNode *c : child_nodes) {
        c->print(depth+1);
    }
}

MCTNode::~MCTNode() {
    parent_node = NULL;
    for(MCTNode *c : child_nodes) {
        delete c;
    }
    MCTS_used_nodes--;
}

void MCTNode::update(U8 result) {
    visits++;
    wins += result;
}

MCTNode * MCTNode::addChild(U8 lmove, Board &board) {
    child_nodes.push_back(new MCTNode(this, board, lmove)); //TODO.. NEED COPY CONSTRUCTOR?
    untried_moves.erase(remove(untried_moves.begin(), untried_moves.end(), lmove), untried_moves.end());
//    for(auto iter = untried_moves.begin(); iter != untried_moves.end(); iter++) {
//        if(*iter == lmove) {
//            untried_moves.erase(iter);
//            break;
//        }
//    }
    return child_nodes.back();
}

MCTNode * MCTNode::selectChildUCT() {
    U8 best = 0;
    MCTNode *child = child_nodes[0];
    double ltvis = log(this->visits);
    double val = (double)child->wins / (double)child->visits + sqrt(2.0 * ltvis / (double)child->visits);

    for(U32 i = 1; i < child_nodes.size(); i++) {
//        cerr << ".";
        child = child_nodes[i];
        double nval = (double)child->wins / (double)child->visits + sqrt(2.0 * ltvis / (double)child->visits);
        if(nval > val) {
            val = nval;
            best = i;
        }
    }
//    double ltvis = log(this->visits);
//    sort(child_nodes.begin(), child_nodes.end(), [&ltvis](const MCTNode *lhs, const MCTNode *rhs) {
//        double lval = (double)lhs->wins / (double)lhs->visits + sqrt(2.0 * ltvis / (double)lhs->visits);
//        double rval = (double)rhs->wins / (double)rhs->visits + sqrt(2.0 * ltvis / (double)rhs->visits);
//        return lval > rval;
//    });

//    return child_nodes[0];

    return child_nodes[best];
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
    U8 getRandomUntried();
    void updateAMAF(U8 idx);
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



void MCTSEngine::updateAMAF(U8 idx) {

}

U8 MCTSEngine::getRandomUntried() {
    int moves = sim_state.possible_moves.size();
    int rand_idx = rand() % moves;
    return sim_state.possible_moves[rand_idx];
}

void MCTSEngine::makePermanentMove(U8 idx) {
    cerr << "Nodes before pruning: " << MCTS_used_nodes << endl;
    root_state.makeMove(idx);

    //prune leftover part of tree
    MCTNode *old_root = root_node;
    root_node = NULL;

    //remove new root from old roots children
    //and set it to new root
    for(auto iter = old_root->child_nodes.begin(); iter != old_root->child_nodes.end(); iter++) {
        if((*iter)->move == idx) {
            //new root
            cerr << "*** OLD" << endl;
            root_node = *iter;
            root_node->parent_node = NULL;
            old_root->child_nodes.erase(iter);
            break;
        }
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

//TODO shuffle vector and get next instead of rand()

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
            U8 nextm = getRandomUntried();
            sim_state.makeMove(nextm);
            node = node->addChild(nextm, sim_state);
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

    for(MCTNode *n : root_node->child_nodes) {
        if(n->visits > most_visits) {
            most_visits = n->visits;
            best_move = n->move;
            wins = n->wins;
        }
    }

    cerr << "### Total Simulations: " << simulations << endl;
    cerr << "Best Move: " << (int) best_move << "   visits: " << most_visits << "   wins: " << wins << " == " << (double)wins/(double)most_visits*100.0 << " %" << endl;

    return best_move;
}


#endif // MCTS_HPP
