/* Jakub Fleiser, 2020 */

#include <ctype.h>
#include <stddef.h>
#include <sys/capability.h>
#include <sys/types.h>

#include <stdio.h>

/*  except - array of capabilites to preserve   */
/*  n - except array length */
/*  drops ALL capabilites from CAP_INHERITABLE  */
/*  drops ALL but except specifed capabilities from CAP_EFFECTIVE and CAP_PERMITTED*/
extern int give_up_capabilities(cap_value_t *except, int n) {
    cap_t caps = cap_get_proc();
    if (caps == NULL)
        return -1;

    // This seems like excessive return value checking
    if (cap_clear_flag(caps, CAP_PERMITTED) == -1) {
        cap_free(caps);
        return -1;
    }
    if (cap_clear_flag(caps, CAP_INHERITABLE) == -1) {
        cap_free(caps);
        return -1;
    }
    if (cap_clear_flag(caps, CAP_EFFECTIVE) == -1) {
        cap_free(caps);
        return -1;
    }
    if (except != NULL) {
        if (cap_set_flag(caps, CAP_PERMITTED, n, except, CAP_SET) == -1) {
            cap_free(caps);
            return -1;
        }
        if (cap_set_flag(caps, CAP_EFFECTIVE, n, except, CAP_SET) == -1) {
            cap_free(caps);
            return -1;
        }
    }

    if (cap_set_proc(caps) == -1) {
        cap_free(caps);
        return -1;
    }

    if (cap_free(caps) == -1)
        return -1;
    return 0;
}
