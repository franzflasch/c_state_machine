#include <stdio.h>
#include <stdint.h>

#define STATE_MACHINE_OK  0
#define STATE_MACHINE_ERR 1

/* 
    This macro expects to be called in a global context.
    It ensures that all states have to be defined. So a NULL state
    is not possible. If some states are not needed for e.g. fail state,
    then you have to define a speficic null_state as a place holder.
 */
#define ADD_STATE(_name, _state_func, _next_state, _fail_state) \
    extern state_machine_state_td _next_state; \
    extern state_machine_state_td _fail_state; \
    state_machine_state_td _name = { #_name, _state_func, &_next_state, &_fail_state }

typedef enum state_machine_ret_code_enum { 
    ok, 
    fail,
    repeat

} state_machine_ret_code;

typedef struct state_machine_state_struct
{
    char *state_name;
    int (* state_func)(void);
    void *next_state;
    void *fail_state;
        
} state_machine_state_td;

typedef struct state_machine_struct
{
    state_machine_state_td *current_state;

    /* methods */
    uint32_t (*run)(struct state_machine_struct *state_machine);
    
} state_machine_td;

static uint32_t state_machine_execute(state_machine_td *state_machine)
{
    state_machine_ret_code rc = 0;
    if(state_machine->current_state->state_func == NULL)
    {
        printf("We got in a null state! Will return to avoid undefined behavior\n");
        return 0;
    }

    printf("Executing state %s\n", state_machine->current_state->state_name);
    rc = state_machine->current_state->state_func();

    if(rc == ok)
    {
        state_machine->current_state = state_machine->current_state->next_state;
    }

    if(rc == fail)
    {
        printf("###### State func returned FAIL ######\n");
        state_machine->current_state = state_machine->current_state->fail_state;
    }

    return 0;
}

uint32_t state_machine_init(state_machine_td *state_machine, state_machine_state_td *entry_state)
{
    state_machine->run = state_machine_execute;
    state_machine->current_state = entry_state;

    return STATE_MACHINE_OK;
}


static int entry_state_func(void) 
{
    printf("we are in %s\n", __func__); 
    return ok; 
}

static int foo_state_func(void) 
{ 
    static int i = 0;
    printf("we are in %s\n", __func__); 
    i++;

    if(i>=5)
    {
        i = 0;
        return fail;
    }

    return ok; 
}

static int bar_state_func(void) 
{ 
    static int i = 0;
    printf("we are in %s\n", __func__); 
    i++;

    if(i>=5)
    {
        i = 0;
        return fail;
    }

    return repeat; 
}

static int fail_state_func(void) 
{ 
    printf("we are in %s\n", __func__); 
    return repeat; 
}

/* This is just a place holder state in case a particular state is not used.
   It should never be reached but it is needed for definition, see down below */
static int null_state_func(void) 
{ 
    printf("NULL STATE! How could this happen!?\n"); 
    return repeat; 
}

/* These are the state machine states. They always have to be defined globally, when using the ADD_STATE macro.
   Dynamic creation could be added later, but it is actually pretty clean to use it this way.
   States can be created easily and it also ensures that all states have to be declared and defined, so
   a real NULL state is actually not possible, thats why we use a special "null_state" as placeholder, if needed.
 */
ADD_STATE(entry_state, entry_state_func, foo_state, null_state);
ADD_STATE(foo_state, foo_state_func, entry_state, bar_state);
ADD_STATE(bar_state, bar_state_func, entry_state, fail_state);
ADD_STATE(fail_state, fail_state_func, entry_state, null_state);
ADD_STATE(null_state, null_state_func, null_state, null_state);

int main(int argc, char *argv[]) 
{
    int i = 0;

    state_machine_td test_state_machine = { 0 };
    state_machine_init(&test_state_machine, &entry_state);

    for(i=0;i<20;i++)
    {
        test_state_machine.run(&test_state_machine);
    }
}
