//
// pch.cpp
//

#include "pch.h"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::FLAGS_gtest_death_test_style = "threadsafe";
	::testing::FLAGS_gtest_break_on_failure = true;
	return RUN_ALL_TESTS();
}

void ExecuteQuery(Qp::IOperator *root, Value *rgvals, ProjectExpression printExpression)
{
	root->Open();

	while (root->GetRow(rgvals))
	{
		if (printExpression)
		{
			printExpression(rgvals);
		}
	}

	root->Close();
}