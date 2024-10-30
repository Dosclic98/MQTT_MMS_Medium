/*
 * NodeContent.h
 *
 *  Created on: Oct 30, 2024
 *      Author: dosclic
 */

#ifndef GRAPH_ATTACK_NODEDEFS_H_
#define GRAPH_ATTACK_NODEDEFS_H_

namespace inet {

enum NodeType {
    AND = 0,
    OR = 1,
    BEGIN = 2,
    END = 3,
    DEFENSE = 4,
    STEP = 5
};

// A kind of attack for each attack step type
enum AttackType {
    NOTSET = -1,
    COMPROMISE = 0,
    SCANIP = 1,
    SCANVULN = 2,
    REMOTESRVC = 3,
    SHELL = 4,
    ADDSSHKEY = 5,
    REMOTESHELL = 6,
    BRUTEFORCE = 7,
    ESCAPEHOST = 8,
    MODAUTHPROC = 9,
    UNSECCRED = 10,
    CREDACC = 11,
    AITM = 12,
    UNAUTHCMDMSG = 13,
    SPOOFREPMSG = 14,
    DERFAILURE = 15
};

struct NodeContent {
    std::string displayName;
    NodeType nodeType;
    AttackType attackType;
    bool state;
    std::string completionDelayExpr;
    std::vector<std::string> targetControllerList;
    std::vector<std::string> children;
};

}; // namespace inet

#endif /* GRAPH_ATTACK_NODEDEFS_H_ */
