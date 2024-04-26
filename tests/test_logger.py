import pytest
import kbmod_logger


def test_python_logging():
    kbmod_logger.module1.run()


def test_cpp_logging():
    kbmod_logger.module2.run()


if __name__ == "__main__":
    test_python_logging()
    print()
    test_cpp_logging()
    #pytest.main()
