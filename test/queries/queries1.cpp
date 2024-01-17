/*
	This file contains more advanced query tests.
	You'll have to implement the query tree yourself for the given task.
*/

#include "pch.h"

// This class contains some mock tables we'll use to run queries on.
//
class QueriesTestSuite : public testing::Test
{
public:
	//	Table Students
	// ------------
	//	StudentId		int
	//	dob				int [YYYYMMDD]
	//	zip				int
	//	phone			int
	//
	struct
	{
		SE::BTree BTree;
		struct
		{
			const unsigned int StudentId = 0;
			const unsigned int Dob = 1;
			const unsigned int Zip = 2;
			const unsigned int Phone = 3;
		} Columns;
	} Students;

	//	Table Teachers
	// ------------
	//	TeacherId		int
	//	dob				int [YYYYMMDD]
	//	zip				int
	//	phone			int
	//
	struct
	{
		SE::BTree BTree;
		struct
		{
			const unsigned int TeacherId = 0;
			const unsigned int Dob = 1;
			const unsigned int Zip = 2;
			const unsigned int Phone = 3;
		} Columns;
	} Teachers;

	//	Table Courses
	// ------------
	//	CourseId		int
	//	TeacherId		int
	//	level			int	[between 1 and 6]
	//
	struct
	{
		SE::BTree BTree;
		struct
		{
			const unsigned int CourseId = 0;
			const unsigned int TeacherId = 1;
			const unsigned int Level = 2;
		} Columns;
	} Courses;

	//	Table Grades
	// ------------
	//	StudentId		int
	//	CourseId		int
	//	date			int	[YYYYMMDD]
	//	grade			int	[between 0 and 100], < 40 means failed
	//
	struct
	{
		SE::BTree BTree;
		struct
		{
			const unsigned int StudentId = 0;
			const unsigned int CourseId = 1;
			const unsigned int Date = 2;
			const unsigned int Grade = 3;
		} Columns;
	} Grades;

protected:
	void SetUp() override
	{
		// Create the students table.
		//
		const int nCols = 3;
		Value rgvals[nCols] = {};

		Students.BTree.InsertRow(/*student_id */10001);
		Students.BTree.InsertRow(10002);
		Students.BTree.InsertRow(10003);
		Students.BTree.InsertRow(10004);
		Students.BTree.InsertRow(10005);
		Students.BTree.InsertRow(10006);
		Students.BTree.InsertRow(10007);
		Students.BTree.InsertRow(10008);
	}
};

// Create a query tree for the following SQL query:
// 
//	SELECT course_id, student_id, grade FROM grades WHERE grade < 40 ORDER BY course_id, student_id
//
TEST_F(QueriesTestSuite, SelectBadGradesByCourseAndStudent)
{
	// WIP
	return;
	Value rgvals[3];

	SE::BTreeSession session(&Grades.BTree);
	Qp::BTreeScanner scanner(&session);
	Qp::Filter filter(&scanner, [=](Value *rgvals) { return rgvals[Grades.Columns.StudentId] < 40; });
	Qp::Sort sort(&filter, 3, [=](Value *l, Value *r)
		{
			if (l[Grades.Columns.CourseId] != r[Grades.Columns.CourseId])
			{
				return l[Grades.Columns.CourseId] < r[Grades.Columns.CourseId];
			};

			return l[Grades.Columns.StudentId] < r[Grades.Columns.StudentId];
		});

	Value numRows = 0;

	ExecuteQuery(&sort, rgvals);

	EXPECT_EQ(1, numRows);
}

