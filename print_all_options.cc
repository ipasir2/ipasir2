#include <stdio.h>
#include <iostream>

#include "ipasir2.h"

using std::cout;
using std::endl;

int main() {
    ipasir2_errorcode err = IPASIR_E_OK;
    void* S;
    err = ipasir2_init(&S);
    if (err) return err;
    else cout << "ipasir2_init() returned " << err << endl;

    ipasir2_option const* option;
    err = ipasir2_options(S, &option);
    if (err) return err;
    else cout << "ipasir2_options() returned " << err << endl;

    while (option->name != nullptr) {
        printf("%s %i %i %i\n", option->name, option->type, option->minimum, option->maximum);
        ++option;
    }

    err = ipasir2_release(S);
    if (err) return err;
    return 0;
}