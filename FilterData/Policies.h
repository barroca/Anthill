#ifndef POLICIES_H
#define POLICIES_H

/* This file represents the policies of the ports.
 * Added by Matheus, sept 13, 2004
 */

/* Output Port */
typedef enum {RANDOM, LABELED_STREAM, MULTICAST_LABELED_STREAM, BROADCAST, ROUND_ROBIN, W_POLICY_ERROR} writePolicy_t;
writePolicy_t getWritePolicyByName(const char *name);

/* Input Port */
typedef enum {FIFO, R_POLICY_ERROR} readPolicy_t;
#endif
