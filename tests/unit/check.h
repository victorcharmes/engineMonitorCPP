#ifndef MONITOR_CHECK_H
#define MONITOR_CHECK_H

// Mini framework de test (un seul fichier, compatible C++98).
// Ne pas modifier.

#include <iostream>
#include <string>

static int g_checks = 0;
static int g_failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        ++g_checks;                                                          \
        if (!(cond)) {                                                       \
            ++g_failures;                                                    \
            std::cout << "  ECHEC " << __FILE__ << ":" << __LINE__           \
                      << "  CHECK(" #cond ")\n";                             \
        }                                                                    \
    } while (0)

// Fonction template : chaque expression n'est evaluee qu'une seule fois.
template <typename A, typename E>
void checkEqual(const A& actual, const E& expected,
                const char* file, int line, const char* text)
{
    ++g_checks;
    if (!(actual == expected)) {
        ++g_failures;
        std::cout << "  ECHEC " << file << ":" << line << "  " << text << "\n"
                  << "        obtenu  : [" << actual << "]\n"
                  << "        attendu : [" << expected << "]\n";
    }
}

#define CHECK_EQ(actual, expected) \
    checkEqual((actual), (expected), __FILE__, __LINE__, #actual)

#define RUN_TEST(fn)                                                         \
    do {                                                                     \
        std::cout << "- " #fn "\n";                                          \
        fn();                                                                \
    } while (0)

inline int checkSummary(const char* suite)
{
    std::cout << suite << " : " << (g_checks - g_failures) << "/" << g_checks
              << " verifications OK\n";
    return (g_failures == 0) ? 0 : 1;
}

#endif
