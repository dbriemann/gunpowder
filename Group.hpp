#ifndef GROUP_H
#define GROUP_H

#include <vector>
using namespace std;

struct Group {
    U8 edge_mask;
    //U8 score;
    vector<U8> members;

    Group();
    void addMember(U8 member);
    void merge(Group &other);
};

Group::Group() {
    edge_mask = 0;
    //score = 0;
    members = vector<U8>();
}

void Group::merge(Group &other) {
    this->edge_mask |= other.edge_mask;
    this->members.assign(other.members.begin(), other.members.end());
    //free unused groups member vector..
    //group cannot be deleted but we save some space
    other.members.clear();
}

void Group::addMember(U8 member) {
    members.push_back(member);
}

#endif // GROUP_H
