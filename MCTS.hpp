#ifndef MCTS_HPP
#define MCTS_HPP

/*
 * TODO
 *
 * -transposition tables?
 * -prior knowledge
 * -remove dead areas / fillin
 * -permutation game sim
 * -memory count(in progress)
 *
 */

#include <vector>
#include <cmath>
#include <unordered_map>
using namespace std;

#include "definitions.hpp"
#include "FastBoard.hpp"
#include "Timer.hpp"

#define PLAIN_NODE_SIZE 90


//TODO NODE MANAGER WITH STATIC MEMORY
static U64 MCTS_used_nodes = 0ULL;
static U64 MCTS_used_bytes = 0ULL;



struct MCTNode {
    U32 wins;
    U32 visits;
    U8 move;
    U8 color;
    vector<U8> untried_moves;
    MCTNode *parent_node;
    unordered_map<U8, MCTNode *> child_nodes;

    MCTNode();
    MCTNode(MCTNode *parent, FastBoard &board, U8 lmove);
    ~MCTNode();
    MCTNode(const MCTNode &other);

    void update(U8 result);
    void explode(FastBoard &board);
    MCTNode * addRandomChild(FastBoard &board);
    MCTNode * selectChildUCT();
    void print(int depth);
};




MCTNode::MCTNode() {
    wins = 0;
    visits = 0;
    move = 0;
    color = BLACK; //color that made "lmove"
    untried_moves = vector<U8>();
    parent_node = NULL;
    child_nodes = unordered_map<U8, MCTNode *>();
    MCTS_used_nodes++;
    MCTS_used_bytes += PLAIN_NODE_SIZE;
}

MCTNode::MCTNode(MCTNode *parent, FastBoard &board, U8 lmove) {
    wins = 0;
    visits = 0;
    move = lmove;
    color = FLIP(board.to_play); //color that made "lmove"
    untried_moves = board.possible_moves;
    //shuffle vector.. so taking and removing a random element is only pop_back stuff in the simulation phase
    random_shuffle(untried_moves.begin(), untried_moves.end());
    parent_node = parent; //NULL if root
    child_nodes = unordered_map<U8, MCTNode *>();
    MCTS_used_nodes++;
    MCTS_used_bytes += PLAIN_NODE_SIZE;
}

MCTNode::MCTNode(const MCTNode &other) {
    wins = other.wins;
    visits = other.visits;
    move = other.move;
    color = other.color;
    untried_moves = other.untried_moves;
    parent_node = other.parent_node;
    child_nodes = other.child_nodes;
    MCTS_used_bytes += PLAIN_NODE_SIZE;
}

MCTNode::~MCTNode() {
    parent_node = NULL;
    for(auto &p : child_nodes) {
        delete p.second;
        p.second = NULL;
    }
    MCTS_used_nodes--;
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

void MCTNode::explode(FastBoard &board) {
    FastBoard cpy;

    for(U8 m : untried_moves) {
        cpy = board;
        cpy.makeMove(m);

        child_nodes[m] = new MCTNode(this, cpy, m);
        child_nodes[m]->visits = 1;
    }

    untried_moves.clear();
    //TODO decrease byte count..
}

MCTNode * MCTNode::addRandomChild(FastBoard &board) {
    U8 lmove = untried_moves.back();
    untried_moves.pop_back();
    board.makeMove(lmove);

    child_nodes[lmove] = new MCTNode(this, board, lmove);
//    child_nodes[lmove]->explode(board);
    return child_nodes[lmove];
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
};

MCTSEngine::MCTSEngine() {
    simulations = 0;
    root_state = FastBoard();
    root_node = new MCTNode(NULL, root_state, 0);
    initRoot(root_state);
}

MCTSEngine::MCTSEngine(FastBoard &game_state) {
    simulations = 0;
    this->root_state = game_state;
    root_node = new MCTNode(NULL, root_state, 0);
    initRoot(root_state);
}


void MCTSEngine::initRoot(FastBoard &rstate) {
//    cerr << "#initRoot()" << endl;

    root_node->untried_moves = rstate.possible_moves;

    //create all possible child nodes for root node
    //this makes amaf updates possible
    for(auto it = root_node->untried_moves.rbegin(); it != root_node->untried_moves.rend(); it++) {
        sim_state = root_state;
        sim_state.makeMove(*it);
        root_node->child_nodes[*it] = new MCTNode(root_node, sim_state, *it);

        //set artificial init values
        //TODO prior knowledge
        root_node->child_nodes[*it]->visits = 1;
//        root_node->child_nodes[*it]->wins = 1;
    }
    root_node->untried_moves.clear();

//    root_node->print(1);
}

void MCTSEngine::makePermanentMove(U8 idx) {
    cerr << "*** Nodes before pruning: " << MCTS_used_nodes << endl;
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

    //node was not expanded
//    if(root_node == NULL) {
//        cerr << "*** NEW NODE AS ROOT" << endl;
//        root_node = new MCTNode(NULL, root_state, idx);
//    }

    initRoot(root_state);

    //clean up memory
    delete old_root;
    old_root = NULL;
    cerr << "*** New root: " << root_node << " move: " << (int)root_node->move << endl;
    cerr << "*** Nodes after pruning: " << MCTS_used_nodes << endl;
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
        while(node->untried_moves.empty() && !node->child_nodes.empty()) {
            //cur_node is fully expanded and non-terminal
            node = node->selectChildUCT();
            sim_state.makeMove(node->move);
        }

        //expansion
        if(!node->untried_moves.empty()) {
            //there are still child nodes to expand
            node = node->addRandomChild(sim_state);
        }

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


#endif // MCTS_HPP
