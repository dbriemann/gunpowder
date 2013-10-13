#ifndef MCTS_HPP
#define MCTS_HPP

#include <vector>
#include <cmath>
using namespace std;

#include "definitions.hpp"
#include "Board.hpp"


struct MCTNode {
    U32 wins;
    U32 visits;
    U8 move;
    U8 color;
    vector<U8> untried_moves;
    MCTNode *parent_node;
    vector<MCTNode> child_nodes;

    MCTNode(MCTNode *parent, Board &board, U8 lmove);
    //TODO COPY CONSTRUCTOR

    void update(U8 result);
    MCTNode * addChild(U8 lmove, Board &board);
    MCTNode * selectChildUCT();
};

MCTNode::MCTNode(MCTNode *parent, Board &board, U8 lmove) {
    wins = 0;
    visits = 0;
    move = lmove;
    color = FLIP(board.to_play); //color that made "lmove"
    untried_moves = board.possible_moves;
    parent_node = parent; //NULL if root
}

void MCTNode::update(U8 result) {
    visits++;
    wins += result;
}

MCTNode * MCTNode::addChild(U8 lmove, Board &board) {
    child_nodes.push_back(MCTNode(this, board, lmove)); //TODO.. NEED COPY CONSTRUCTOR?
    untried_moves.erase(remove(untried_moves.begin(), untried_moves.end(), lmove), untried_moves.end());
    return &child_nodes.back();
}

MCTNode * MCTNode::selectChildUCT() {
    double ltvis = log(this->visits);
    sort(child_nodes.begin(), child_nodes.end(), [&ltvis](const MCTNode &lhs, const MCTNode &rhs) {
        double lval = (double)lhs.wins / (double)lhs.visits + sqrt(2.0 * ltvis / (double)lhs.visits);
        double rval = (double)rhs.wins / (double)rhs.visits + sqrt(2.0 * ltvis / (double)rhs.visits);
        return lval > rval;
    });

    return &child_nodes[0];
}



struct MCTS {
    MCTS();

    void runUCT(Board &root); //TODO MAX TIME etc
};

MCTS::MCTS() {
}

void MCTS::runUCT(Board &root) {
    MCTNode root_node(NULL, root, 0);
    U64 simulations = 0ULL;

    //for now never stop searching
    while(true) {
        simulations++;
        MCTNode *cur_node = &root_node;
        Board state = root;

        //prior knowledge?!

        //selection
        while(cur_node->untried_moves.empty() && !cur_node->child_nodes.empty()) {
            //cur_node is fully expanded and non-terminal
            cur_node = cur_node->selectChildUCT();
            state.makeMove(cur_node->move);
        }

        //expansion
        if(!cur_node->untried_moves.empty()) {
            //there are still child nodes to expand
            U8 nextm = state.getRandomMove();
            state.makeMove(nextm);
            cur_node = cur_node->addChild(nextm, state);
        }

        //simulation
        I8 win = NONE;
        while(!state.possible_moves.empty() && win == NONE) {
            win = state.makeMove(state.getRandomMove());
        }

        //backpropagation
        while(cur_node != NULL) {
            if(win == WHITE_WIN) {
                if(cur_node->color == WHITE) {
                    cur_node->update(1);
                } else {
                    cur_node->update(0);
                }
            } else { //win black
                if(cur_node->color == BLACK) {
                    cur_node->update(1);
                } else {
                    cur_node->update(0);
                }
            }

            cur_node = cur_node->parent_node;
        }

        if(simulations % 1000 == 0) {
            U32 most_visits = 0;
            U8 best_move = 0;
            U32 wins = 0;

            for(MCTNode &n : root_node.child_nodes) {
                if(n.visits > most_visits) {
                    most_visits = n.visits;
                    best_move = n.move;
                    wins = n.wins;
                }
            }

            cerr << "Best Move: " << (int) best_move << "   visits: " << most_visits << "   wins: " << wins << endl;
        }
    }
}

//def UCT(rootstate, itermax, verbose = False):
//    """ Conduct a UCT search for itermax iterations starting from rootstate.
//        Return the best move from the rootstate.
//        Assumes 2 alternating players (player 1 starts), with game results in the range [0.0, 1.0]."""

//    rootnode = Node(state = rootstate)

//    for i in range(itermax):
//        node = rootnode
//        state = rootstate.Clone()

//        # Select
//        while node.untriedMoves == [] and node.childNodes != []: # node is fully expanded and non-terminal
//            node = node.UCTSelectChild()
//            state.DoMove(node.move)

//        # Expand
//        if node.untriedMoves != []: # if we can expand (i.e. state/node is non-terminal)
//            m = random.choice(node.untriedMoves)
//            state.DoMove(m)
//            node = node.AddChild(m,state) # add child and descend tree

//        # Rollout - this can often be made orders of magnitude quicker using a state.GetRandomMove() function
//        while state.GetMoves() != []: # while state is non-terminal
//            state.DoMove(random.choice(state.GetMoves()))

//        # Backpropagate
//        while node != None: # backpropagate from the expanded node and work back to the root node
//            node.Update(state.GetResult(node.playerJustMoved)) # state is terminal. Update node with result from POV of node.playerJustMoved
//            node = node.parentNode

//    # Output some information about the tree - can be omitted
//    if (verbose): print rootnode.TreeToString(0)
//    else: print rootnode.ChildrenToString()

//    return sorted(rootnode.childNodes, key = lambda c: c.visits)[-1].move # return the move that was most visited

//def UCTPlayGame():
//    """ Play a sample game between two UCT players where each player gets a different number
//        of UCT iterations (= simulations = tree nodes).
//    """
//    # state = OthelloState(4) # uncomment to play Othello on a square board of the given size
//    # state = OXOState() # uncomment to play OXO
//    state = NimState(15) # uncomment to play Nim with the given number of starting chips
//    while (state.GetMoves() != []):
//        print str(state)
//        if state.playerJustMoved == 1:
//            m = UCT(rootstate = state, itermax = 1000, verbose = False) # play with values for itermax and verbose = True
//        else:
//            m = UCT(rootstate = state, itermax = 100, verbose = False)
//        print "Best Move: " + str(m) + "\n"
//        state.DoMove(m)
//    if state.GetResult(state.playerJustMoved) == 1.0:
//        print "Player " + str(state.playerJustMoved) + " wins!"
//    elif state.GetResult(state.playerJustMoved) == 0.0:
//        print "Player " + str(3 - state.playerJustMoved) + " wins!"
//    else: print "Nobody wins!"

#endif // MCTS_HPP
