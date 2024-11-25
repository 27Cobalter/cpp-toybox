#include <gtest/gtest.h>

#include <string>
#include <ostream>

#include <unordered_map>

enum class ResultCpp {
	Success,
	Error,
};

enum ResultC {
	ResultC_Success,
	ResultC_Error,
};

class ResultWrapper {
private:
	std::unordered_map<ResultC, std::string> table = { {ResultC::ResultC_Success, "ResultC_Success"}, {ResultC::ResultC_Error, "ResultC_Error"} };
public:
	ResultC c_;
	ResultWrapper(ResultC c) : c_(c) {};
	std::string ToString() { return table[c_]; }
};

enum Type {
	Type_TypeA,
	Type_TypeB,
};

std::ostream& operator<<(std::ostream& os, const ResultC& c)
{
	os << ResultWrapper(c).ToString();
	return os;
}

std::ostream& operator<<(std::ostream& os, const ResultWrapper& r)
{
	os << r.c_;
	return os;
}

bool operator==(const ResultWrapper& lvalue, const ResultC& rvalue)
{
	return lvalue.c_ == rvalue;
}

TEST(TestCaseName, TestName) {
	EXPECT_EQ(ResultWrapper(ResultC_Success), ResultC::ResultC_Success);
	EXPECT_EQ(ResultWrapper(ResultC_Error), ResultC::ResultC_Success);
	EXPECT_EQ(ResultC::ResultC_Success, ResultC::ResultC_Error);
	EXPECT_EQ(Type::Type_TypeA, Type::Type_TypeB);
}