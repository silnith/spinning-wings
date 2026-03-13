#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include "CppUnitTest.h"

#include "GLInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std::literals::string_literals;

namespace silnith::wings::gl::tests
{
    TEST_CLASS(GLInfoTests)
    {
    public:

        TEST_METHOD(TestStringVersion)
        {
            std::string const version{ "2.3.1"s };
            GLInfo const info{ version };

            Assert::AreEqual(GLuint{ 2 }, info.getMajorVersion());
            Assert::AreEqual(GLuint{ 3 }, info.getMinorVersion());
        }

        TEST_METHOD(TestCStringVersion)
        {
            char const* const version{ "2.3.1" };
            GLInfo const info{ version };

            Assert::AreEqual(GLuint{ 2 }, info.getMajorVersion());
            Assert::AreEqual(GLuint{ 3 }, info.getMinorVersion());
        }

    };
}
