#include "CppUnitTest.h"

#include "Color.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace silnith::wings::tests
{
	TEST_CLASS(ColorTests)
	{
	public:
		
		TEST_METHOD(TestBlackFloatRedComponentIsZero)
		{
			Assert::AreEqual(0.0f, Color<float>::BLACK.getRed(), 0.0f);
		}

		TEST_METHOD(TestBlackFloatGreenComponentIsZero)
		{
			Assert::AreEqual(0.0f, Color<float>::BLACK.getGreen(), 0.0f);
		}

		TEST_METHOD(TestBlackFloatBlueComponentIsZero)
		{
			Assert::AreEqual(0.0f, Color<float>::BLACK.getBlue(), 0.0f);
		}

		TEST_METHOD(TestBlackDoubleRedComponentIsZero)
		{
			Assert::AreEqual(0.0, Color<double>::BLACK.getRed(), 0.0);
		}

		TEST_METHOD(TestBlackDoubleGreenComponentIsZero)
		{
			Assert::AreEqual(0.0, Color<double>::BLACK.getGreen(), 0.0);
		}

		TEST_METHOD(TestBlackDoubleBlueComponentIsZero)
		{
			Assert::AreEqual(0.0, Color<double>::BLACK.getBlue(), 0.0);
		}

		TEST_METHOD(TestWhiteFloatRedComponentIsOne)
		{
			Assert::AreEqual(1.0f, Color<float>::WHITE.getRed(), 0.0f);
		}

		TEST_METHOD(TestWhiteFloatGreenComponentIsOne)
		{
			Assert::AreEqual(1.0f, Color<float>::WHITE.getGreen(), 0.0f);
		}

		TEST_METHOD(TestWhiteFloatBlueComponentIsOne)
		{
			Assert::AreEqual(1.0f, Color<float>::WHITE.getBlue(), 0.0f);
		}

		TEST_METHOD(TestWhiteDoubleRedComponentIsOne)
		{
			Assert::AreEqual(1.0, Color<double>::WHITE.getRed(), 0.0);
		}

		TEST_METHOD(TestWhiteDoubleGreenComponentIsOne)
		{
			Assert::AreEqual(1.0, Color<double>::WHITE.getGreen(), 0.0);
		}

		TEST_METHOD(TestWhiteDoubleBlueComponentIsOne)
		{
			Assert::AreEqual(1.0, Color<double>::WHITE.getBlue(), 0.0);
		}
	};
}
