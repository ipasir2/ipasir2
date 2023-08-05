/**
* MIT License
* 
* @file ipasir1ipasir2.cc
* @author Markus Iser(markus.iser@kit.edu)
* @brief Wrap IPASIR-2 solver in IPASIR-1 app
* @date 2023-08-03
* 
* This file is part of IPASIR-2.
* 
*/

#include "ipasir.h"
#include "ipasir2.h"

#include <new>
#include <stdexcept>
#include <vector>


namespace {
using learn_callback_fn = void(*)(void* data, int32_t* clause);

class ipasir2_solver
{
public:
    ipasir2_solver() {
        if (ipasir2_init(&m_solver) != IPASIR_E_OK) {
            throw std::runtime_error("ipasir_init() failed");
        }
    }

    ~ipasir2_solver() {
        if (m_solver != nullptr) {
            ipasir2_release(m_solver);
        }
    }

    void add(int32_t lit_or_zero) {
        ipasir2_add(m_solver, lit_or_zero);
    }

    void assume(int32_t lit) {
        ipasir2_assume(m_solver, lit);
    }

    int32_t solve() {
        int32_t result = 0;
        if (ipasir2_solve(m_solver, &result) == IPASIR_E_OK) {
            return result;
	}
        return 0;
    }

    int32_t val(int32_t lit) {
        int32_t result = 0;
        ipasir2_val(m_solver, lit, &result);
        return result;
    }

    int32_t failed(int32_t lit) {
        int32_t result = 0;
        ipasir2_failed(m_solver, lit, &result);
        return result;
    }

    void set_terminate(void* data, int(*terminate)(void* data)) {
        ipasir2_set_terminate(m_solver, data, terminate);
    }

    void set_learn(void* data, learn_callback_fn learn_fn) {
        m_learn_callback = learn_fn;
        m_learn_callback_data = data;

	// The noexcept specifier is a quickfix, making sure that no exception is thrown into the
	// callback's callsite (which might not be C++). However, this causes std::terminate to be
	// called instead. Possible alternatives:
	// - keep a failure flag and return 0 from solve() if that flag is true
	// - or silently dropping learnt clauses when an error occurs
        ipasir2_set_learn(m_solver, static_cast<void*>(this), [](void* data, int32_t const* clause) noexcept {
            ipasir2_solver* this_ = static_cast<ipasir2_solver*>(data);
            this_->copy_learnt_clause(clause);
            this_->m_learn_callback(this_->m_learn_callback_data, this_->m_last_learnt_clause.data());
        });
    }

private:
    void copy_learnt_clause(int32_t const* clause) {
        int32_t const* cursor = clause;
        m_last_learnt_clause.clear();

        while(*cursor != 0) {
            m_last_learnt_clause.push_back(*cursor);
            ++cursor;
        }

        m_last_learnt_clause.push_back(0);
    }

    void* m_solver = nullptr;

    learn_callback_fn m_learn_callback = nullptr;
    void* m_learn_callback_data = nullptr;
    std::vector<int> m_last_learnt_clause;
};


ipasir2_solver* to_ipasir2(void* solver) {
    return static_cast<ipasir2_solver*>(solver);
}
}

const char* ipasir_signature() {
    char const* result;
    ipasir2_signature(&result);
    return result;
}

void* ipasir_init() {
    try {
        return static_cast<void*>(new ipasir2_solver());
    }
    catch(std::exception const&) {
        return nullptr;
    }
}

void ipasir_release(void* solver) {
    delete to_ipasir2(solver);
}

void ipasir_add(void* solver, int32_t lit_or_zero) {
    to_ipasir2(solver)->add(lit_or_zero);
}

void ipasir_assume(void* solver, int32_t lit) {
    to_ipasir2(solver)->assume(lit);
}

int ipasir_solve(void* solver) {
    return to_ipasir2(solver)->solve();
}

int32_t ipasir_val(void* solver, int32_t lit) {
    return to_ipasir2(solver)->val(lit);
}

int ipasir_failed(void* solver, int32_t lit) {
    return to_ipasir2(solver)->failed(lit);
}

void ipasir_set_terminate(void* solver, void* data, int(*terminate)(void* data)) {
    to_ipasir2(solver)->set_terminate(data, terminate);
}

void ipasir_set_learn(void* solver, void* data, int /*max_length*/, void(*learned)(void* data, int32_t* clause)) {
    to_ipasir2(solver)->set_learn(data, learned);
}

