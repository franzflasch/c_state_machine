/*
http://stackoverflow.com/questions/1371460/state-machines-tutorials/1371654#1371654
State machines are very simple in C if you use function pointers.
Basically you need 2 arrays - one for state function pointers and one for state 
transition rules. Every state function returns the code, you lookup state 
transition table by state and return code to find the next state and then 
just execute it.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

enum ret_codes { ok, fail, repeat};


/* Within these functions the states can be handled 
 * depending on the return code (ok, fail, repeat) 
 * the destination state will be chosen by the function lookup_transition.
*/
int entry_state(void) 
{
    printf("we are in %s\n", __FUNCTION__); 
    return ok; 
}

int foo_state(void) 
{ 
    printf("we are in %s\n", __FUNCTION__); 
    return ok; 
};

int bar_state(void) 
{ 
    printf("we are in %s\n", __FUNCTION__); 
    return repeat; 
};

int exit_state(void) 
{ 
    printf("we are in %s\n", __FUNCTION__); 
    return ok; 
};

/* array and enum below must be in sync! */
int (* state[])(void) = { entry_state, foo_state, bar_state, exit_state};
enum state_codes { entry, foo, bar, end};

struct transition {
    enum state_codes src_state;
    enum ret_codes   ret_code;
    enum state_codes dst_state;
};

/* transitions from end state aren't needed */
struct transition state_transitions[] = {
    {entry, ok,     foo},
    {entry, fail,   end},
    {foo,   ok,     bar},
    {foo,   fail,   end},
    {foo,   repeat, foo},
    {bar,   ok,     end},
    {bar,   fail,   end},
    {bar,   repeat, foo}
};
#define STATE_TRANSITIONS_ARR_SIZE (sizeof(state_transitions)/sizeof(struct transition))

#define ENTRY_STATE entry
#define EXIT_STATE end

enum ret_codes lookup_transitions(enum state_codes cur_state, enum ret_codes rc)
{
    int i = 0;

    for(i=0;i<STATE_TRANSITIONS_ARR_SIZE;i++)
    {
        if((state_transitions[i].src_state == cur_state) && (state_transitions[i].ret_code == rc))
        {
            printf("Found next state!\n");
            break;
        }
    }

    /* Check if we actually found a matching state */
    if(i == STATE_TRANSITIONS_ARR_SIZE)
    {
        printf("Could not found matching dst state!\n");
        assert(i != STATE_TRANSITIONS_ARR_SIZE);
    }

    return state_transitions[i].dst_state;
}

int main(int argc, char *argv[]) 
{
    enum state_codes cur_state = ENTRY_STATE;
    enum ret_codes rc;
    int (* state_fun)(void);

    for (;;) {
        state_fun = state[cur_state];
        rc = state_fun();
        if (EXIT_STATE == cur_state)
            break;
        cur_state = lookup_transitions(cur_state, rc);
    }

    return EXIT_SUCCESS;
}
