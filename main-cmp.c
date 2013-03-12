/*
 * main.c
 *
 *  Created on: Mar 7, 2012
 *      Author: andrew.b.clark
 */

#include "md5-test.h"

#include <CUnit/Basic.h>
#include <stdlib.h>

int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
	{
		return CU_get_error();
	}

	/* add a suite to the registry */
	pSuite = CU_add_suite("MD5-test", NULL, NULL);

	if (pSuite)
	{
		/* add the tests to the suite */
		if (!(CU_ADD_TEST(pSuite, digest_test) == NULL ||
	          CU_ADD_TEST(pSuite, _IETF_test) == NULL ||
	          1 == 0))
		{
			/* Run all tests using the CUnit Basic interface */
			CU_basic_set_mode(CU_BRM_VERBOSE);
			CU_basic_run_suite(pSuite);
		}
	}

	CU_cleanup_registry();

	return CU_get_error();

}
