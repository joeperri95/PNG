#include "test.h"
#include "test_bitstream.h"
#include "test_huffman.h"
#include "test_png.h"

test_results run_suite(test_suite suite)
{

    test_results res =
    {
        .num_passed = 0,
        .num_failed = 0,
        .num_total = suite.number_of_tests,
    };

    printf("Running test suite: %s\n", suite.suite_name);
    for(int i = 0; i < suite.number_of_tests; i++)
    {
       if(suite.test_list[i].procedure())
       {
            printf("Test %s " ANSI_COLOR_GREEN "PASSED\n" ANSI_COLOR_RESET, suite.test_list[i].test_name);
            res.num_passed++;
       }
       else
       {
            printf("Test %s " ANSI_COLOR_RED "FAILED\n" ANSI_COLOR_RESET, suite.test_list[i].test_name);
            res.num_failed++;
       }
    }
    
    if(res.num_passed == res.num_total)
    {
        printf("All %d tests " ANSI_COLOR_GREEN "PASSED\n" ANSI_COLOR_RESET, res.num_total);
    }
    else if(res.num_failed == res.num_total)
    {
        printf("All %d tests " ANSI_COLOR_RED "FAILED\n" ANSI_COLOR_RESET, res.num_total);
    }
    else
    {
        printf("Tests complete: %d tests " ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET " %d tests "
        ANSI_COLOR_RED "FAILED\n" ANSI_COLOR_RESET , res.num_passed, res.num_failed);
    }

    return res;
}



int  main(int argc, char *argv)
{
    printf("%d\n", LOGGING_ENABLED);
    extern test_suite bitstream_suite; 
    extern test_suite huffman_suite;    
    extern test_suite inflate_suite;

    test_results  to_date;
    
    test_results total = {
        .num_passed = 0,
        .num_failed = 0,
        .num_total  = 0,
    };

    to_date = run_suite(bitstream_suite); 
    total.num_passed += to_date.num_passed;
    total.num_failed += to_date.num_failed; 
    total.num_total += to_date.num_total; 

    to_date = run_suite(huffman_suite);
    total.num_passed += to_date.num_passed;
    total.num_failed += to_date.num_failed; 
    total.num_total += to_date.num_total; 
    
    to_date = run_suite(inflate_suite);
    total.num_passed += to_date.num_passed;
    total.num_failed += to_date.num_failed; 
    total.num_total += to_date.num_total; 

    printf("All %d tests complete: %d tests " ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET " %d tests "
    ANSI_COLOR_RED "FAILED\n" ANSI_COLOR_RESET , total.num_total, total.num_passed, total.num_failed);

    return 0;

}
