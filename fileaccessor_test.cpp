#include "fileaccessor.hpp"

#include <cassert>


using std::unique_ptr;
using std::istream;
using std::ofstream;


static void testFakeFilesWithNonExistingFile()
{
  FakeFiles files;
  unique_ptr<istream> maybe_stream = files.maybeOpenForRead("test.dat");
  assert(!maybe_stream);
}


static void testFakeFilesWithExistingFile()
{
  FakeFiles files;
  files.store("test.dat","x");
  unique_ptr<istream> maybe_stream = files.maybeOpenForRead("test.dat");
  assert(maybe_stream);
  assert(maybe_stream->peek()=='x');
}


static void testSystemFilesWithNonExistingFile()
{
  SystemFiles files;
  unique_ptr<istream> maybe_stream =
    files.maybeOpenForRead("nosuchdirectory/nosuchfile.dat");
  assert(!maybe_stream);
}


static void testSystemFilesWithExistingFile()
{
  SystemFiles files;

  {
    ofstream stream("storagehandlertest.dat");
    stream << "x";
  }

  unique_ptr<istream> maybe_stream =
    files.maybeOpenForRead("storagehandlertest.dat");
  assert(maybe_stream);
  assert(maybe_stream->peek()=='x');
}


int main()
{
  testFakeFilesWithNonExistingFile();
  testFakeFilesWithExistingFile();
  testSystemFilesWithNonExistingFile();
  testSystemFilesWithExistingFile();
}
